// Copyright 2017-2022 Jordon Brooks.

#include "Player/BaseMotionController.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Items/BasicItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Player/BasePlayerPawn.h"
#include "Player/VRPlayerController.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseMotionController, All, All)


ABaseMotionController::ABaseMotionController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionControllerComponent->SetupAttachment(SceneComponent);

	GhostHandMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostHandMesh"));
	GhostHandMeshComponent->SetupAttachment(MotionControllerComponent);

	HandMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMeshComponent->SetupAttachment(SceneComponent);

	WristComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Wrist"));
	WristComponent->SetupAttachment(HandMeshComponent);

	SteamVRChaperoneComp = CreateDefaultSubobject<UBoxComponent>(TEXT("SteamVRChaperone"));

	AttachComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttachComponent"));
	AttachComponent->SetupAttachment(HandMeshComponent);
	AttachComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachComponent->SetSphereRadius(2.0f);

	ItemRotationComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ItemRotation"));
	ItemRotationComponent->SetupAttachment(AttachComponent);
	ItemRotationComponent->ArrowSize = 0.31f;

	GrabSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereComponent"));
	GrabSphereComponent->SetupAttachment(HandMeshComponent);
	GrabSphereComponent->SetSphereRadius(5.0f);
	GrabSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrabSphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GrabSphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GrabSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
	GrabSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	GrabSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECollisionResponse::ECR_Block);
	GrabSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECollisionResponse::ECR_Block);
	GrabSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECollisionResponse::ECR_Ignore);
	GrabSphereComponent->ComponentTags.Add(FName("IgnoreHandCollision"));

	PhysicsHandler = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandler->SetLinearDamping(100.0f);
	PhysicsHandler->SetLinearStiffness(5000.0f);
	PhysicsHandler->SetAngularDamping(100.0f);
	PhysicsHandler->SetAngularStiffness(5000.0f);
	PhysicsHandler->SetInterpolationSpeed(50.0f);

	WristComponent->SetSphereRadius(3.0f);
	WristComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	WristComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WristComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WristComponent->ComponentTags.Add(FName("IgnoreHandCollision"));

	WristComponent->SetRelativeLocationAndRotation(FVector(-16.05f, 4.40f, 3.50f), FRotator(-45.0f, 0.0f, 0.0f));
	GhostHandMeshComponent->SetRelativeLocation(FVector(3.0f, -3.0f, 0.0f));
	GrabSphereComponent->SetRelativeLocation(FVector(-9.09408f, -0.681107f, -2.33113f));
	ItemRotationComponent->SetRelativeLocationAndRotation(FVector(-4.2f, 1.0f, -2.8f), FRotator(0.0f, 0.0f, -45.0f));

	TeleportCylinderComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCylinder"));
	TeleportCylinderComp->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>TeleportCylinderObj(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	TeleportCylinderComp->SetStaticMesh(TeleportCylinderObj.Object);
	TeleportCylinderComp->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.0f));
	TeleportCylinderComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TeleportCylinderComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TeleportCylinderComp->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>SplineMaterialObj(TEXT("Material'/VREngine/Materials/VR/M_SplineArcMat.M_SplineArcMat'"));
	SplineMaterial = SplineMaterialObj.Object;


	RingComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>RingObj(TEXT("StaticMesh'/VREngine/Meshes/SM_FatCylinder.SM_FatCylinder'"));
	RingComp->SetupAttachment(TeleportCylinderComp);
	RingComp->SetStaticMesh(RingObj.Object);
	RingComp->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.15f));
	RingComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RingComp->SetMobility(EComponentMobility::Movable);

	ArrowComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>ArrowObj(TEXT("StaticMesh'/VREngine/Meshes/BeaconDirection.BeaconDirection'"));
	ArrowComp->SetupAttachment(TeleportCylinderComp);
	ArrowComp->SetStaticMesh(ArrowObj.Object);
	ArrowComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArrowComp->SetMobility(EComponentMobility::Movable);

	RoomScaleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomScaleMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>RoomScaleMeshObj(TEXT("StaticMesh'/VREngine/Meshes/1x1_cube.1x1_cube'"));
	RoomScaleMeshComp->SetupAttachment(ArrowComp);
	RoomScaleMeshComp->SetStaticMesh(RoomScaleMeshObj.Object);
	RoomScaleMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RoomScaleMeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RoomScaleMeshComp->SetMobility(EComponentMobility::Movable);

	ArcSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	ArcSplineComp->SetupAttachment(HandMeshComponent);
	ArcSplineComp->SetRelativeLocation(FVector(10.0f, 0.0f, 2.0f));
	ArcSplineComp->SetMobility(EComponentMobility::Movable);

	ArcEndPointComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndPoint"));
	ArcEndPointComp->SetupAttachment(HandMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>ArcEndPointObj(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	ArcEndPointComp->SetStaticMesh(ArcEndPointObj.Object);
	ArcEndPointComp->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
	ArcEndPointComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArcEndPointComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ArcEndPointComp->SetMobility(EComponentMobility::Movable);

	ArcDirectionComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArcDirection"));
	ArcDirectionComp->SetupAttachment(HandMeshComponent);
	ArcDirectionComp->SetRelativeLocation(FVector(10.0f, 0.0f, 0.0f));
	ArcDirectionComp->ArrowSize = 0.2f;
	ArcDirectionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ArcDirectionComp->SetMobility(EComponentMobility::Movable);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(GhostHandMeshComponent);
	PhysicsConstraint->SetDisableCollision(true);
	PhysicsConstraint->ConstraintInstance.ProfileInstance.ProjectionLinearTolerance = 2000.0f;
	PhysicsConstraint->ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Free;
	PhysicsConstraint->ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Free;
	PhysicsConstraint->ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Free;
	PhysicsConstraint->SetLinearPositionDrive(true, true, true);
	PhysicsConstraint->SetLinearDriveParams(5000.0f, 1500.0f, 0.0f);
	PhysicsConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	PhysicsConstraint->SetAngularOrientationDrive(true, true);
	PhysicsConstraint->SetAngularVelocityDrive(true, true);
	PhysicsConstraint->SetAngularDriveParams(500.0f, 50.0f, 0.0f);
	PhysicsConstraint->ConstraintInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked);
	PhysicsConstraint->ConstraintInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked);


	static ConstructorHelpers::FObjectFinder<USkeletalMesh>HandMeshObject(TEXT("SkeletalMesh'/VREngine/Meshes/Mannequin_Hands/Character/Mesh/MannequinHand_Right.MannequinHand_Right'"));
	USkeletalMesh* HandMesh = HandMeshObject.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SplineMeshObject(TEXT("StaticMesh'/VREngine/Meshes/BeamMesh.BeamMesh'"));
	SplineMesh = SplineMeshObject.Object;

	HandMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GhostHandMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//Set blueprint animation
	HandMeshComponent->SetSkeletalMesh(HandMesh);
	HandMeshComponent->SetEnableGravity(false);
	HandMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel7);
	HandMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel7, ECollisionResponse::ECR_Ignore);
	HandMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel8, ECollisionResponse::ECR_Ignore);

	GhostHandMeshComponent->SetSkeletalMesh(HandMesh);
	GhostHandMeshComponent->SetEnableGravity(false);
	GhostHandMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostHandMeshComponent->SetVisibility(false, false);

	SetMCSource();

	if (ControllerType == EMotionControllerType::ValveIndex || ControllerType == EMotionControllerType::HTCVive)
	{
		WristComponent->SetRelativeLocationAndRotation(FVector(-13.266188f, 1.339187f, 4.882311f), FRotator(0.0f, 0.0f, -40.0));
		//HandMeshComponent->SetRelativeLocationAndRotation(FVector(3.0f, -3.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		GhostHandMeshComponent->SetRelativeLocationAndRotation(FVector(3.0f, -3.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		GrabSphereComponent->SetRelativeLocationAndRotation(FVector(-4.226496f, -0.369772f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	}

	HandMeshComponent->OnComponentHit.AddDynamic(this, &ABaseMotionController::HandComponentHit);

	GrabSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseMotionController::GrabsphereComponentBeginOverlap);
	GrabSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseMotionController::GrabsphereComponentEndOverlap);

	ControllerType = GetControllerType();
	//SW_LOG("Hit Event Bind!");
}

void ABaseMotionController::RumbleController(UHapticFeedbackEffect_Curve* HapticEffect, float Intensity, bool bLoop)
{
	GetWorld()->GetFirstPlayerController()->PlayHapticEffect(HapticEffect, HandType, Intensity, bLoop);
}

void ABaseMotionController::StopRumbleController()
{
	GetWorld()->GetFirstPlayerController()->StopHapticEffect(HandType);
}

void ABaseMotionController::SetMCSource()
{
	if (HandType == EControllerHand::Left) MotionControllerComponent->SetTrackingMotionSource(TEXT("Left"));
	else MotionControllerComponent->SetTrackingMotionSource(TEXT("Right"));
}

void ABaseMotionController::BeginPlay()
{
	Super::BeginPlay();

	//PlayerController = UGlobalFunctionLibrary::GetPlayerController(GetWorld());

	if (ABasePlayerPawn* p = Cast<ABasePlayerPawn>(GetInstigator())) Player = p;
	else UE_LOG(LogBaseMotionController, Fatal, TEXT("Instigator is not child of ABasePlayerPawn!"));

	PlayerController = Player->PlayerController;

	EnableInput(GetWorld()->GetFirstPlayerController());

	if (MotionControllerComponent->IsTracked())
	{
		SetupRoomScaleOutline();
	}

	TeleportCylinderComp->SetVisibility(false, true);

	RoomScaleMeshComp->SetVisibility(false, false);

	ArcEndPointComp->SetVisibility(false, true);

	const FVector InvertVec = FVector(1.0f, -1.0f, 1.0f);

	switch (HandType)
	{
	case EControllerHand::Left:
		GrabSphereComponent->SetRelativeScale3D(InvertVec);
		ItemRotationComponent->SetRelativeScale3D(InvertVec);
		HandMeshComponent->SetRelativeScale3D(InvertVec);
		GhostHandMeshComponent->SetRelativeScale3D(InvertVec);
		GrabSphereComponent->AddWorldRotation(FRotator(-90, 0, 180));
		break;

	default:
		break;
	}

	switch (PlayerController->ControllerPhysics)
	{
	case EControllerPhysicsType::PhysicsConstraint:
		HandMeshComponent->AttachToComponent(GhostHandMeshComponent, 
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
										EAttachmentRule::KeepWorld, true));
		HandMeshComponent->SetEnableGravity(false);
		HandMeshComponent->SetNotifyRigidBodyCollision(true);
		PhysicsConstraint->SetConstrainedComponents(GhostHandMeshComponent, FName("hand_r"),
											HandMeshComponent, FName("hand_r"));
		//HandMeshComponent->SetSimulatePhysics(true); // BUG: Breaks the joints of player's hands when simulating physics is set to true
		HandMeshComponent->GetBodyInstance("hand_r")->SetInstanceSimulatePhysics(true);
		UpdateHandPhysics();
		break;
	default:
		break;
	}

}

void ABaseMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(PlayerController)) // Only run if not simulating in editor
	{
		switch (PlayerController->ControllerPhysics)
		{
		case EControllerPhysicsType::NoCollision:
			HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(),
													false, nullptr, ETeleportType::TeleportPhysics);
			break;
		case EControllerPhysicsType::Physics:
			UpdateHandPhysics();
			break;
		case EControllerPhysicsType::Stop:
			break;
		default:
			break;
		}

		if (!bHasGrabbedObject)
		{
			const FNearestActorRet NearestObject = GetActorNearHand();

			if (IsValid(NearestObject.NearestActor))
			{
				HoveringItem = NearestObject.NearestActor;
				DoOnce_HasGrabbedActorA_Func();
				DoOnce_HasGrabbedActorB_FuncReset();
			}
			else
			{
				DoOnce_HasGrabbedActorB_Func();
				DoOnce_HasGrabbedActorA_FuncReset();
			}
			ClearArc();

			if (bIsTeleporterActive)
			{
				TArray<FVector> TracePoints;
				FVector NavMeshLocation;
				FVector TraceLocation;

				const bool IsValidDestination = TraceTeleportDest(TracePoints, NavMeshLocation, TraceLocation);

				bIsValidTeleportDestination = IsValidDestination;

				TeleportCylinderComp->SetVisibility(bIsValidTeleportDestination, true);

				FVector TraceEnd = FVector(NavMeshLocation);
				TraceEnd.Z -= 200;

				TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

				FHitResult HitResult;

				UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), NavMeshLocation, TraceEnd, ObjectTypes, true, TArray<AActor*>(),
					bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResult, true);

				TeleportCylinderComp->SetWorldLocation(UKismetMathLibrary::SelectVector(HitResult.ImpactPoint, NavMeshLocation, HitResult.bBlockingHit));

				if (UKismetMathLibrary::BooleanOR(UKismetMathLibrary::BooleanAND(bIsValidTeleportDestination, !bLastFrameValidDestination),
												UKismetMathLibrary::BooleanAND(bLastFrameValidDestination, !bIsValidTeleportDestination)))
				{
					if (IsValid(RumbleHapticEffect))
					{
						RumbleController(RumbleHapticEffect, 0.3f, false);
					}
				}
				bLastFrameValidDestination = bIsValidTeleportDestination;

				UpdateArcSpline(bIsValidTeleportDestination, TracePoints);

				UpdateArcEndpoint(TraceLocation, bIsValidTeleportDestination);

				UpdateRoomScaleOutline();
			}
		}
		UpdateGhostHand();
	}
}

void ABaseMotionController::HandComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{
		const TSubclassOf<UDamageType> ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);

		const float Damage = (FMath::Abs(Velocity) * 100) * DamageMultiplier;

		const IDamage* Interface = Cast<IDamage>(OtherActor);

		if (OtherActor->GetClass()->ImplementsInterface(UDamage::StaticClass()))
		{
			Interface->Execute_RecieveDamage(OtherActor, Damage, Hit);
		}
	}
}

void ABaseMotionController::SetupRoomScaleOutline()
{
	FVector2D OutRectCenter;
	FTransform OutRectTransform;
	UHeadMountedDisplayFunctionLibrary::GetPlayAreaRect(OutRectTransform, OutRectCenter);

	SteamVRChaperoneComp->SetWorldTransform(OutRectTransform);

}

void ABaseMotionController::UpdateGhostHand()
{
	//The same effect can be achieved by using Physics Constraints
	const FVector LocalCurrentPosition = HandMeshComponent->GetComponentLocation();

	Velocity = (LastFramePosition.Size() - LocalCurrentPosition.Size());

	LastFramePosition = LocalCurrentPosition;

	if (UpdateTimer <= 0.0f)
	{
		UpdateTimer = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	}
	else
	{
		if (UpdateTimer + (1 / HandLocationUpdateFPS) <= UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()))
		{
			UpdateTimer = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

			TArray<UPrimitiveComponent*> Overlaps;

			TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
			//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));


			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);
			ActorsToIgnore.Add((AActor*)Player);
			if (IsValid(AttachedItem)) ActorsToIgnore.Add((AActor*)AttachedItem);

			//HandCollisionComponent->GetOverlappingComponents(Overlaps);
			UKismetSystemLibrary::ComponentOverlapComponents(GhostHandMeshComponent, GhostHandMeshComponent->GetComponentTransform(), ObjectTypes,
											TSubclassOf<UPrimitiveComponent>(), ActorsToIgnore, Overlaps);
			if (bUseGhostMesh)
			{
				if (Overlaps.Num() > 0)
				{
					GhostHandMeshComponent->SetVisibility(true, true);
				}
				else
				{
					GhostHandMeshComponent->SetVisibility(false, false);
					HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(),
															false, nullptr, ETeleportType::TeleportPhysics);
				}
			}
			else
			{
				HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(),
														false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}

void ABaseMotionController::UpdateHandPhysics() const
{
	HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(),
	                                               GhostHandMeshComponent->GetComponentRotation(), true, nullptr,
	                                               ETeleportType::TeleportPhysics);
}

void ABaseMotionController::DoOnce_HasGrabbedActorA_Func()
{
	if (!DoOnce_HasGrabbedActorA)
	{
		if (const IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HoveringItem))
		{
			InteractionInterface->Execute_HoverBegin(HoveringItem, this);
			DoOnce_HasGrabbedActorA = true;
			UE_LOG(LogBaseMotionController, Verbose, TEXT("Hover Begin: %s"), *HoveringItem->GetHumanReadableName());
		}
	}
}

void ABaseMotionController::UpdateRoomScaleOutline() const
{
	if (RoomScaleMeshComp->IsVisible())
	{
		FVector HMDPos;
		FRotator HMDRot;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

		RoomScaleMeshComp->SetRelativeLocation(UKismetMathLibrary::Quat_UnrotateVector(FQuat(FRotator(0.0f, HMDRot.Yaw, 0.0f)),
											UKismetMathLibrary::Divide_VectorFloat(FVector(HMDPos.X, HMDPos.Y, 0), -1)));
	}
}

EMotionControllerType ABaseMotionController::GetControllerType()
{
	UE_LOG(LogBaseMotionController, Verbose, TEXT("Detecting controller type"));
	FString ControllerTypeString;
	
	UMotionControllerComponent* MC = CreateDefaultSubobject<UMotionControllerComponent>(
		TEXT("MotionController_Detect")); // This method can cause potential lag if abused on tick!
	MC->bDisplayDeviceModel = true;

	if (IsValid(MC->CustomDisplayMesh)) // Returns Null if no controllers are present
		ControllerTypeString = MC->CustomDisplayMesh->GetFName().ToString().ToLower();

	MC->DestroyComponent();

	if (ControllerTypeString.IsEmpty())
	{
		UE_LOG(LogBaseMotionController, Warning, TEXT("WARNING: I don't think you have your controllers switched on."));
		return EMotionControllerType::NoController;
	}
	
	if (ControllerTypeString.Contains("SamsungOdyssey", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Samsung Odyssey controllers."));
		return EMotionControllerType::SamsungOdyssey;
	}

	if (ControllerTypeString.Contains("SamsungGear", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Samsung Gear controllers."));
		return EMotionControllerType::ValveIndex;
	}

	if (ControllerTypeString.Contains("PicoNeo", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the PicoNeo controllers."));
		return EMotionControllerType::PicoNeo;
	}
	
	if (ControllerTypeString.Contains("PicoG2", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the PicoG2 controllers."));
		return EMotionControllerType::PicoG2;
	}
	
	if (ControllerTypeString.Contains("OculusTouch_v3", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Oculus Touch controllers."));
		return EMotionControllerType::OculusTouch_v3;
	}
	
	if (ControllerTypeString.Contains("OculusTouch_v2", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Oculus Touch controllers."));
		return EMotionControllerType::OculusTouch_v2;
	}
	
	if (ControllerTypeString.Contains("OculusTouch", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Oculus Touch controllers."));
		return EMotionControllerType::OculusTouch;
	}
	
	if (ControllerTypeString.Contains("indexcontroller", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Valve Index controllers."));
		return EMotionControllerType::ValveIndex;
	}
	
	if (ControllerTypeString.Contains("MicrosoftMixedReality", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Microsoft Mixed Reality controllers."));
		return EMotionControllerType::WindowsMixedReality;
	}
	
	if (ControllerTypeString.Contains("MagicLeapOne", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Microsoft Magic Leap One controllers."));
		return EMotionControllerType::MagicLeapOne;
	}
	
	if (ControllerTypeString.Contains("HTCViveFocusPlus", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the HTC Vive Focus Plus controllers."));
		return EMotionControllerType::HTCViveFocusPlus;
	}
	
	if (ControllerTypeString.Contains("HTCViveFocus", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the HTC Vive Focus controllers."));
		return EMotionControllerType::HTCViveFocus;
	}
	
	if (ControllerTypeString.Contains("HTCViveCosmos", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the HTC Vive Cosmos controllers."));
		return EMotionControllerType::HTCViveCosmos;
	}
	
	if (ControllerTypeString.Contains("HTCVive", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the HTC Vive controllers."));
		return EMotionControllerType::HTCVive;
	}
	
	if (ControllerTypeString.Contains("HPMixedReality", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the HP Mixed Reality controllers."));
		return EMotionControllerType::HTCVive;
	}
	
	if (ControllerTypeString.Contains("GoogleDaydream", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogBaseMotionController, Verbose, TEXT("INFO: I think your using the Google Daydream controllers."));
		return EMotionControllerType::GoogleDaydream;
	}

	
	UE_LOG(LogBaseMotionController, Warning, TEXT("WARNING: I can't seem to detect what controller your using, try manually setting ControllerType."));
	return EMotionControllerType::Unknown;
}
