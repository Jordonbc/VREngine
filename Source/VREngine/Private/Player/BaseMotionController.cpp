// Copyright 2017-2020 Jordon Brooks.

#include "Player/BaseMotionController.h"
#include "Player/BasePlayerPawn.h"
#include "UObject\UObjectIterator.h"
#include "Interfaces/VRDualHands.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
//#include "Project_IgniteV2\Project_IgniteV2.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GlobalFunctionLibrary.h"
#include "Items/BasicItem.h"
//#include "DebugMacros.h"
#include "GameFramework/DamageType.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/Damage.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Components/SplineMeshComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Player/VRPlayerController.h"
#include "HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "SteamVRChaperoneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(MotionController, All, All)


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

	SteamVRChaperoneComp = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("SteamVRChaperone"));

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

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>SplineMaterialObj(TEXT("Material'/Game/GlobalMaterials/VR/M_SplineArcMat.M_SplineArcMat'"));
	SplineMaterial = SplineMaterialObj.Object;
	

	RingComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>RingObj(TEXT("StaticMesh'/Game/VirtualReality/Meshes/SM_FatCylinder.SM_FatCylinder'"));
	RingComp->SetupAttachment(TeleportCylinderComp);
	RingComp->SetStaticMesh(RingObj.Object);
	RingComp->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.15f));
	RingComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RingComp->SetMobility(EComponentMobility::Movable);

	ArrowComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>ArrowObj(TEXT("StaticMesh'/Game/VirtualReality/Meshes/BeaconDirection.BeaconDirection'"));
	ArrowComp->SetupAttachment(TeleportCylinderComp);
	ArrowComp->SetStaticMesh(ArrowObj.Object);
	ArrowComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArrowComp->SetMobility(EComponentMobility::Movable);

	RoomScaleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomScaleMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>RoomScaleMeshObj(TEXT("StaticMesh'/Game/VirtualReality/Meshes/1x1_cube.1x1_cube'"));
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


	static ConstructorHelpers::FObjectFinder<USkeletalMesh>HandMeshObject(TEXT("SkeletalMesh'/Game/VirtualReality/Mannequin/Character/Mesh/MannequinHand_Right.MannequinHand_Right'"));
	USkeletalMesh* HandMesh = HandMeshObject.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SplineMeshObject(TEXT("StaticMesh'/Game/VirtualReality/Meshes/BeamMesh.BeamMesh'"));
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
	//SW_LOG("Hit Event Bind!");
}

void ABaseMotionController::GrabActor()
{
	bWantsToGrip = true;
	FNearestActorRet NearestActor = GetActorNearHand();
	if ((IsValid(NearestActor.NearestActor)))
	{
		UE_LOG(MotionController, Verbose, TEXT("Grabbing %s"), *NearestActor.NearestActor->GetName());
		GrabSpecificActor(NearestActor.NearestActor, NearestActor.InteractedComponent);
	}
	else
	{
		UE_LOG(MotionController, Verbose, TEXT("Grabbing AIR"));
	}
	

	
}

void ABaseMotionController::ReleaseActor()
{
	bWantsToGrip = false;
	if (IsValid(AttachedItem))
	{
		if (AttachedItem->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			UE_LOG(MotionController, Verbose, TEXT("Releasing %s"), *AttachedItem->GetName());

			if (IVRDualHands* VRDualHandsInterface = Cast<IVRDualHands>(AttachedItem))
			{
				if (!IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
				{
					VRDualHandsInterface->Execute_SecondHand_Drop(AttachedItem);
					bHasGrabbedObject = false;
				}
				else
				{
					if (IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
					{
						IInteractionInterface::Execute_Drop(AttachedItem);
						UE_LOG(MotionController, Verbose, TEXT("Releasing Actor: %s"), *AttachedItem->GetHumanReadableName());
					}
				}
			}
			else
			{
				if (IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
				{
					IInteractionInterface::Execute_Drop(AttachedItem);
					UE_LOG(MotionController, Verbose, TEXT("Releasing Actor: %s"), *AttachedItem->GetHumanReadableName());
				}
			}

			bHasGrabbedObject = false;
			RumbleController(RumbleHapticEffect, 0.2f, false);

			AttachedItem = NULL;
			bIsSecondHand = false;
		}
	}
}

FNearestActorRet ABaseMotionController::GetActorNearHand()
{
	FNearestActorRet NearestActor = FNearestActorRet();

	for (int i = 0; i < OverlappingComponents.Num(); i++)
	{
		// Filter out objects not contained in the target world.
		if (OverlappingComponents[i]->GetWorld() != GetWorld())
		{
			continue;
		}
		// Do stuff
		AActor* Actor = OverlappingComponents[i]->GetOwner();

		if (OverlappingComponents[i]->ComponentHasTag(FName("SecondHand")))
		{
			if (Actor->GetClass()->ImplementsInterface(UVRDualHands::StaticClass()))
			{
				float Vect = FVector::Distance(Actor->GetActorLocation(), GrabSphereComponent->GetComponentLocation());
				if (Vect < NearestActor.DistanceToActor)
				{
					NearestActor.NearestActor = Actor;
					NearestActor.InteractedComponent = OverlappingComponents[i];
					NearestActor.DistanceToActor = Vect;
				}
			}
		}
		else
		{
			if (OverlappingComponents[i]->ComponentHasTag(FName("Grip")))
			{
				float Vect = FVector::Distance(Actor->GetActorLocation(), GrabSphereComponent->GetComponentLocation());
				if (Vect < NearestActor.DistanceToActor)
				{
					NearestActor.NearestActor = Actor;
					NearestActor.InteractedComponent = OverlappingComponents[i];
					NearestActor.DistanceToActor = Vect;
				}
			}
		}
	}

	return NearestActor;
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

void ABaseMotionController::GetTeleportDestination(FVector &Location, FRotator &Rotation)
{
	Location = TeleportCylinderComp->GetComponentLocation();

	Rotation = TeleportRotation;
}

void ABaseMotionController::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGlobalFunctionLibrary::GetPlayerController(GetWorld());

	if (ABasePlayerPawn* p = Cast<ABasePlayerPawn>(GetInstigator())) Player = p;
	else UE_LOG(MotionController, Fatal, TEXT("Instigator is not child of ABasePlayerPawn!"));

	EnableInput(GetWorld()->GetFirstPlayerController());

	SetupRoomScaleOutline();

	TeleportCylinderComp->SetVisibility(false, true);

	RoomScaleMeshComp->SetVisibility(false, false);

	ArcEndPointComp->SetVisibility(false, true);

	FVector InvertVec = FVector(1.0f, -1.0f, 1.0f);

	switch (HandType)
	{
	case EControllerHand::Left:
		GrabSphereComponent->SetRelativeScale3D(InvertVec);
		ItemRotationComponent->SetRelativeScale3D(InvertVec);
		HandMeshComponent->SetRelativeScale3D(InvertVec);
		GhostHandMeshComponent->SetRelativeScale3D(InvertVec);
		GrabSphereComponent->AddWorldRotation(FRotator(-90, 0 , 180));
		break;

	default:
		break;
	}

}

void ABaseMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UGlobalFunctionLibrary::IsEditorSimulating()) // Only run if not simulating in editor
	{
		switch (PlayerController->ControllerPhysics)
		{
		case EControllerPhysicsType::NoCollision:
			HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(), false, nullptr, ETeleportType::TeleportPhysics);
			break;
		case EControllerPhysicsType::Physics:
			UpdateHandPhysics();
			break;
		case EControllerPhysicsType::Stop:
			UpdateGhostHand();
			break;
		default:
			break;
		}

		if (!bHasGrabbedObject)
		{
			FNearestActorRet NearestObject;

			NearestObject = GetActorNearHand();

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
				bool IsValidDestination;

				IsValidDestination = TraceTeleportDest(TracePoints, NavMeshLocation, TraceLocation);

				bIsValidTeleportDestination = IsValidDestination;

				TeleportCylinderComp->SetVisibility(bIsValidTeleportDestination, true);

				FVector TraceEnd = FVector(NavMeshLocation);
				TraceEnd.Z -= 200;

				TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

				FHitResult HitResult;

				UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), NavMeshLocation, TraceEnd, ObjectTypes, true, TArray<AActor*>(), bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResult, true);

				TeleportCylinderComp->SetWorldLocation(UKismetMathLibrary::SelectVector(HitResult.ImpactPoint, NavMeshLocation, HitResult.bBlockingHit));

				if (UKismetMathLibrary::BooleanOR(UKismetMathLibrary::BooleanAND(bIsValidTeleportDestination, !bLastFrameValidDestination), UKismetMathLibrary::BooleanAND(bLastFrameValidDestination, !bIsValidTeleportDestination)))
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
	}
}

void ABaseMotionController::HandComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{
		//SW_LOG("HIT!!!!");
		//SW_LOG(OtherActor->GetName());
		TSubclassOf<UDamageType> ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);

		float Damage = (FMath::Abs(Velocity) * 100) * DamageMultiplier;

		IDamage* Interface = Cast<IDamage>(OtherActor);

		if (OtherActor->GetClass()->ImplementsInterface(UDamage::StaticClass()))
		{
			Interface->Execute_RecieveDamage(OtherActor, Damage, Hit);
		}
	}
}

void ABaseMotionController::GrabsphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(MotionController, Verbose, TEXT("Detected object: %s"), *OtherComp->GetName());
	if (OtherActor != (AActor*)Player || OtherActor != this)
	{
		OverlappingComponents.Add(OtherComp);
	}
}

void ABaseMotionController::GrabsphereComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingComponents.Remove(OtherComp);
}

void ABaseMotionController::SetupRoomScaleOutline()
{
	float ChaperoneMeshHeight = 70.0f;
	FVector OutRectCenter;
	FRotator OutRectRotation;
	float OutX;
	float OutY;
	UKismetMathLibrary::MinimumAreaRectangle(GetWorld(), SteamVRChaperoneComp->GetBounds(), FVector(0.0f, 0.0f, 1.0f), OutRectCenter, OutRectRotation, OutX, OutY);

	bIsRoomScale = UKismetMathLibrary::BooleanNAND(UKismetMathLibrary::NearlyEqual_FloatFloat(OutX, 100), UKismetMathLibrary::NearlyEqual_FloatFloat(OutY, 100));

	if (bIsRoomScale)
	{
		RoomScaleMeshComp->SetWorldScale3D(FVector(OutX, OutY, ChaperoneMeshHeight));
		RoomScaleMeshComp->SetWorldRotation(OutRectRotation);
	}

}

void ABaseMotionController::ActivateTeleporter()
{
	bIsTeleporterActive = true;
	TeleportCylinderComp->SetVisibility(true, true);
	RoomScaleMeshComp->SetVisibility(bIsRoomScale, false);
	InitialControllerRotation = MotionControllerComponent->GetComponentRotation();
}

void ABaseMotionController::DisableTeleporter()
{
	if (bIsTeleporterActive)
	{
		bIsTeleporterActive = false;
		TeleportCylinderComp->SetVisibility(false, true);
		ArcEndPointComp->SetVisibility(false, false);
		RoomScaleMeshComp->SetVisibility(false, false);
	}
}

void ABaseMotionController::ClearArc()
{
	for (size_t i = 0; i < SplineMeshes.Num(); i++)
	{
		SplineMeshes[i]->DestroyComponent();
	}
	SplineMeshes.Empty();
	ArcSplineComp->ClearSplinePoints();
}

void ABaseMotionController::UpdateArcSpline(bool FoundValidLocation, TArray<FVector> SplinePoints)
{
	if (!FoundValidLocation)
	{
		SplinePoints.Empty();
		SplinePoints.Add(ArcDirectionComp->GetComponentLocation());
		SplinePoints.Add(ArcDirectionComp->GetComponentLocation() + (ArcDirectionComp->GetForwardVector() * 20));
	}
	for (size_t i = 0; i < SplinePoints.Num(); i++)
	{
		ArcSplineComp->AddSplinePoint(SplinePoints[i], ESplineCoordinateSpace::Local, true);
	}

	ArcSplineComp->SetSplinePointType(SplinePoints.Num(), ESplinePointType::CurveClamped, true);

	for (size_t i = 0; i < (ArcSplineComp->GetNumberOfSplinePoints() - (int32)2); i++)
	{
		USplineMeshComponent* smc = NewObject<USplineMeshComponent>(this);
		smc->SetStaticMesh(SplineMesh);
		smc->SetMobility(EComponentMobility::Movable);
		smc->SetMaterial(0, SplineMaterial);
		smc->RegisterComponent();
		smc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		smc->SetStartAndEnd(SplinePoints[i], ArcSplineComp->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local), SplinePoints[i + 1], ArcSplineComp->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local), true);

		SplineMeshes.Add(smc);
	}
}

void ABaseMotionController::UpdateArcEndpoint(FVector NewLocation, bool HasValidLocation)
{
	ArcEndPointComp->SetVisibility(UKismetMathLibrary::BooleanAND(HasValidLocation, bIsTeleporterActive), false);
	ArcEndPointComp->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);
	ArcEndPointComp->SetWorldRotation(UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0.0f, HMDRot.Yaw, 0.0f)), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);

	RoomScaleMeshComp->SetWorldRotation(TeleportRotation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void ABaseMotionController::UpdateGhostHand()
{
	FVector LocalCurrentPosition = HandMeshComponent->GetComponentLocation();

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
			UKismetSystemLibrary::ComponentOverlapComponents(GhostHandMeshComponent, GhostHandMeshComponent->GetComponentTransform(), ObjectTypes, TSubclassOf<UPrimitiveComponent>(), ActorsToIgnore, Overlaps);
			if (bUseGhostMesh)
			{
				if (Overlaps.Num() > 0)
				{
					GhostHandMeshComponent->SetVisibility(true, true);
				}
				else
				{
					GhostHandMeshComponent->SetVisibility(false, false);
					HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(), false, nullptr, ETeleportType::TeleportPhysics);
				}
			}
			else
			{
				HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(), false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}

void ABaseMotionController::UpdateHandPhysics()
{
	HandMeshComponent->SetWorldLocationAndRotation(GhostHandMeshComponent->GetComponentLocation(), GhostHandMeshComponent->GetComponentRotation(), true);
}

void ABaseMotionController::DoOnce_HasGrabbedActorA_Func()
{
	if (!DoOnce_HasGrabbedActorA)
	{
		if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HoveringItem))
		{
			InteractionInterface->Execute_HoverBegin(HoveringItem, this);
			DoOnce_HasGrabbedActorA = true;
			UE_LOG(MotionController, Verbose, TEXT("Hover Begin: %s"), *HoveringItem->GetHumanReadableName());
		}
	}
}

void ABaseMotionController::DoOnce_HasGrabbedActorB_Func()
{
	if (!DoOnce_HasGrabbedActorB)
	{
		if (IsValid(HoveringItem))
		{
			if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HoveringItem))
			{
				InteractionInterface->Execute_HoverEnd(HoveringItem, this);
				UE_LOG(MotionController, Verbose, TEXT("Hover End: %s"), *HoveringItem->GetHumanReadableName());
				HoveringItem = nullptr;
			}
		}
		DoOnce_HasGrabbedActorB = true;
	}
}

void ABaseMotionController::DoOnce_HasGrabbedActorA_FuncReset()
{
	DoOnce_HasGrabbedActorA = false;
}

void ABaseMotionController::DoOnce_HasGrabbedActorB_FuncReset()
{
	DoOnce_HasGrabbedActorB = false;
}

void ABaseMotionController::UpdateRoomScaleOutline()
{
	if (RoomScaleMeshComp->IsVisible())
	{
		FVector HMDPos;
		FRotator HMDRot;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

		RoomScaleMeshComp->SetRelativeLocation(UKismetMathLibrary::Quat_UnrotateVector(FQuat(FRotator(0.0f, HMDRot.Yaw, 0.0f)), UKismetMathLibrary::Divide_VectorFloat(FVector(HMDPos.X, HMDPos.Y, 0), -1)));
	}
}

bool ABaseMotionController::GrabSpecificActor(AActor* ActorToGrab, USceneComponent* InteractedComponent)
{
	if (IsValid(ActorToGrab))
	{
		if (ActorToGrab->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			if (ActorToGrab->GetClass()->ImplementsInterface(UVRDualHands::StaticClass()))
			{
				if (IVRDualHands::Execute_HasFirstHand(ActorToGrab))
				{
					IVRDualHands::Execute_SecondHand_Pickup(ActorToGrab, AttachComponent, this);

					bIsSecondHand = true;
					UE_LOG(MotionController, Verbose, TEXT("Second hand grabbing Specific Actor: %s"), *ActorToGrab->GetName());

				}
				else
				{
					bIsSecondHand = false;
					IInteractionInterface::Execute_Pickup(ActorToGrab, this, (bool)InteractedComponent->ComponentHasTag(FName("Center")));

					UE_LOG(MotionController, Verbose, TEXT("Grabbing Specific Actor: %s"), *ActorToGrab->GetName());
				}
			}
			else
			{
				bIsSecondHand = false;
				IInteractionInterface::Execute_Pickup(ActorToGrab, this, (bool)InteractedComponent->ComponentHasTag(FName("Center")));

				UE_LOG(MotionController, Verbose, TEXT("Grabbing Specific Actor: %s"), *ActorToGrab->GetName());

			}

			RumbleController(RumbleHapticEffect, 0.5f, false);

			AttachedItem = ActorToGrab;
			bHasGrabbedObject = true;

			return true;
		}
	}

	return false;
}

bool ABaseMotionController::TraceTeleportDest(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation)
{
	TracePoints.Empty();

	float ProjectNavExtends = 500.0f;
	FVector ProjectedLocation;

	FPredictProjectilePathParams Params = FPredictProjectilePathParams();
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	Params.bTraceComplex = true;
	Params.StartLocation = ArcDirectionComp->GetComponentLocation();
	Params.LaunchVelocity = UKismetMathLibrary::Multiply_VectorFloat(ArcDirectionComp->GetForwardVector(), Player->TeleportLaunchVelocity);
	Params.bTraceWithCollision = true;
	Params.DrawDebugType = bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	Params.ActorsToIgnore.Add(this);
	Params.ActorsToIgnore.Add((AActor*)Player);


	FPredictProjectilePathResult Result = FPredictProjectilePathResult();

	bool Success1 = UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);

	for (size_t i = 0; i < Result.PathData.Num(); i++)
	{
		TracePoints.Add(Result.PathData[i].Location);
	}

	
	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);


	TSubclassOf<UNavigationQueryFilter> FilterClass = 0;

	bool Success2 = UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), Result.HitResult.Location, ProjectedLocation, NavData, FilterClass, FVector(ProjectNavExtends));

	NavMeshLocation = ProjectedLocation;
	TraceLocation = Result.HitResult.Location;

	bool Success = UKismetMathLibrary::BooleanAND(Success1, Success2);

	UE_LOG(MotionController, VeryVerbose, TEXT("Teleport collision is %s"), Success ? TEXT("True") : TEXT("False"));

	return Success;
}