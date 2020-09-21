// Copyright 2017-2020 Jordon Brooks.

#include "Player/BasePlayerPawn.h"
#include "Player/BaseMotionController.h"
#include "Items/BasicItem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
//#include "Log.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MotionControllerComponent.h"
//#include "Project_IgniteV2.h"
#include "Player/VRPlayerController.h"
#include "GlobalFunctionLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/WidgetComponent.h"

DEFINE_LOG_CATEGORY_STATIC(Player, All, All)

// Sets default values
ABasePlayerPawn::ABasePlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	VROriginComp = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	BodyComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	HeadComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HUDComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HUD"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));

	SetRootComponent(DefaultSceneRootComp);

	VROriginComp->SetupAttachment(DefaultSceneRootComp);
	CameraComp->SetupAttachment(VROriginComp);
	SpringArmComp->SetupAttachment(CameraComp);
	BodyComp->SetupAttachment(VROriginComp);
	HeadComp->SetupAttachment(BodyComp);
	HUDComp->SetupAttachment(SpringArmComp);
	CapsuleComponent->SetupAttachment(VROriginComp);

	CapsuleComponent->SetRelativeLocation(FVector(-9.445213f, 0.0f, 88.0f));
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetSimulatePhysics(true);
	//CapsuleComponent->SetMassOverrideInKg(FName(""), 1000000000.0f);
	
	//Workaround: SetMassOverrideInKg() fails when packaging
	CapsuleComponent->BodyInstance.bOverrideMass = true;
	CapsuleComponent->BodyInstance.SetMassOverride(1000000000.0f);

	CapsuleComponent->SetLinearDamping(1000000000.0f);
	CapsuleComponent->SetAngularDamping(1000000000.0f);
	CapsuleComponent->SetEnableGravity(true);



	static ConstructorHelpers::FObjectFinder<USkeletalMesh>BodyObject(TEXT("SkeletalMesh'/Game/Meshes/Characters/testCharacter/SK_Mannequin_Headless.SK_Mannequin_Headless'"));
	BodyComp->SetSkeletalMesh(BodyObject.Object);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>HeadObject(TEXT("SkeletalMesh'/Game/Meshes/Characters/testCharacter/Head/SK_Mannequin_Head.SK_Mannequin_Head'"));
	HeadComp->SetSkeletalMesh(HeadObject.Object);
	HeadComp->SetRelativeLocationAndRotation(FVector(0.0f, -2.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	SpringArmComp->TargetArmLength = 50.0f;
	SpringArmComp->bDoCollisionTest = false;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraRotationLagSpeed = 5.0f;
	SpringArmComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 180.0f));

	HUDComp->SetDrawSize(FVector2D(1920.0f, 1080.0f));
	HUDComp->SetDrawAtDesiredSize(true);
	HUDComp->SetWindowFocusable(false);
	HUDComp->SetEnableGravity(false);
	HUDComp->bApplyImpulseOnDamage = false;
	HUDComp->SetGenerateOverlapEvents(false);
	HUDComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	HUDComp->SetBlendMode(EWidgetBlendMode::Transparent);
	HUDComp->SetTwoSided(true);

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ABasePlayerPawn::CapsuleComponentBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ABasePlayerPawn::CapsuleComponentEndOverlap);
}

void ABasePlayerPawn::ActivateTeleporter(ABaseMotionController* MC)
{
	//ULog::Info("ActivatingTeleporter", ELoggingOptions::LO_Console, true);
	
	if (PlayerController->bCanTeleport && IsValid(MC))
	{
		MC->ActivateTeleporter();
		if (MC == LeftController)
		{
			RightController->DisableTeleporter();
		}
		else
		{
			if (MC == RightController)
			{
				LeftController->DisableTeleporter();
			}
		}
	}
}

void ABasePlayerPawn::StartPlayerMovement(class ABaseMotionController* MC)
{
	//ULog::Info("StartPlayerMovement");
	switch (PlayerController->MovementType)
	{
	case EMoveMode::Teleport:
		ActivateTeleporter(MC);
		break;
	case EMoveMode::Locomotion:
		break;
	default:
		break;
	}
}

void ABasePlayerPawn::StopPlayerMovement(class ABaseMotionController* MC)
{
	UE_LOG(Player, Verbose, TEXT("StopPlayerMovement"));
	switch (PlayerController->MovementType)
	{
	case EMoveMode::Teleport:
		ExecuteTeleporter(MC);
		break;
	case EMoveMode::Locomotion:
		break;
	default:
		break;
	}
}

void ABasePlayerPawn::ExecuteTeleporter(ABaseMotionController* MC)
{

	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (MC->bIsTeleporterActive) ExecuteTeleportation(MC);
		}
	}
}

void ABasePlayerPawn::PostPlayerTeleportation()
{
	//SW_LOG("POST-TELEPORT!");
	TeleportMotionController->DisableTeleporter();
	FVector DestLoc;
	FRotator DestRot;

	TeleportMotionController->GetTeleportDestination(DestLoc, DestRot);
	CapsuleComponent->SetWorldLocationAndRotation(UKismetMathLibrary::Add_VectorVector(DestLoc, FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight())), DestRot, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);

	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	CameraManager->StartCameraFade(0.0f, 1.0f, PlayerController->FadeDuration, FLinearColor(0, 0, 0, 1));
	PlayerController->bIsTeleporting = false;
	UpdateActorLocation();
}

bool ABasePlayerPawn::IsControllerType(ABaseMotionController* MC, EMotionControllerType MCType)
{
	return MC->ControllerType == MCType;
}

void ABasePlayerPawn::GrabObject(ABaseMotionController* MC)
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (IsControllerType(MC, EMotionControllerType::ValveIndex))
			{
				MC->GrabActor();
			}
			else
			{
				if (MC->bWantsToGrip) MC->ReleaseActor();
				else MC->GrabActor();
			}
		}
	}
}

void ABasePlayerPawn::ReleaseObject(ABaseMotionController* MC)
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (IsControllerType(MC, EMotionControllerType::ValveIndex))
			{
				MC->ReleaseActor();
			}
		}
	}
}

// Called when the game starts or when spawned
void ABasePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGlobalFunctionLibrary::GetPlayerController(GetWorld());

	CapsuleComponent->SetVisibility(PlayerController->bCollisionDebugMode ? true : false, true);
	CapsuleComponent->SetHiddenInGame(PlayerController->bCollisionDebugMode ? true : false, true);
	
	GetWorldTimerManager().SetTimer(IKFootTickFunc, this, &ABasePlayerPawn::IKFootTick, GetInverseKinematicFPS(), true);
	if (bUseOldTracking) GetWorldTimerManager().SetTimer(UpdateHandIKFunc, this, &ABasePlayerPawn::UpdateHandIK, GetInverseKinematicFPS(), true);

	FString DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();

	if (DeviceName.Contains("OculusHMD") || DeviceName.Contains("SteamVR"))
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else if (DeviceName.Contains("PSVR"))
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		VROriginComp->AddLocalOffset(FVector(0.0f, 0.0f, PlayerController->DefaultPlayerHeight));
		PlayerController->bUseControllerRollToRotate = true;
	}

	if (PlayerController->MovementType == EMoveMode::Locomotion)
	{
		bUseControllerRotationYaw = true;
	}

	LeftController = SpawnMotionController(EControllerHand::Left);
	RightController = SpawnMotionController(EControllerHand::Right);
}

// Called every frame
void ABasePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector VecAdd = UKismetMathLibrary::Add_VectorVector(CameraComp->GetComponentLocation(), UKismetMathLibrary::Multiply_VectorFloat(CameraComp->GetForwardVector(), -20));
	BodyComp->SetWorldLocation(FVector(VecAdd.X, VecAdd.Y, CameraComp->GetComponentLocation().Z - 170), (FHitResult*)false, nullptr, ETeleportType::TeleportPhysics);

	UpdateRoomScalePosition();

	if (PlayerController->MovementType == EMoveMode::Teleport)
	{
		CheckCharacterPosition();
	}
	else
	{
		FVector loc = FVector(CameraComp->GetComponentLocation().X, CameraComp->GetComponentLocation().Y, CameraComp->GetComponentLocation().Z - 90);
		CapsuleComponent->SetWorldLocation(loc);
		switch (PlayerController->MovementDirection)
		{
		case EMoveDirection::Camera:
			switch (PlayerController->ControllerMovementInput)
			{
			case EControllerSelection::Left:
				AddMovementInput(FVector(CameraComp->GetForwardVector().X, CameraComp->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(CameraComp->GetRightVector().X, CameraComp->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_X") * PlayerController->LocomotionSpeed);
				AddControllerYawInput(GetInputAxisValue("MotionControllerThumbRight_X"));
				break;
			case EControllerSelection::Right:
				AddMovementInput(FVector(CameraComp->GetForwardVector().X, CameraComp->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(CameraComp->GetRightVector().X, CameraComp->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_X") * PlayerController->LocomotionSpeed);
				AddControllerYawInput(GetInputAxisValue("MotionControllerThumbLeft_X"));
				break;
			case EControllerSelection::Both:
				AddMovementInput(FVector(CameraComp->GetForwardVector().X, CameraComp->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(CameraComp->GetRightVector().X, CameraComp->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_X") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(CameraComp->GetForwardVector().X, CameraComp->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(CameraComp->GetRightVector().X, CameraComp->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_X") * PlayerController->LocomotionSpeed);
				AddControllerYawInput(GetInputAxisValue("MotionControllerThumbLeft_X"));
				AddControllerYawInput(GetInputAxisValue("MotionControllerThumbRight_X"));
				break;
			default:
				break;
			}
			break;
		case EMoveDirection::Controller:
			switch (PlayerController->ControllerMovementInput)
			{
			case EControllerSelection::Left:
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetForwardVector().X, LeftController->MotionControllerComponent->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetRightVector().X, LeftController->MotionControllerComponent->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_X") * PlayerController->LocomotionSpeed);
				break;
			case EControllerSelection::Right:
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetForwardVector().X, LeftController->MotionControllerComponent->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetRightVector().X, LeftController->MotionControllerComponent->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_X") * PlayerController->LocomotionSpeed);
				break;
			case EControllerSelection::Both:
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetForwardVector().X, LeftController->MotionControllerComponent->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetRightVector().X, LeftController->MotionControllerComponent->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbLeft_X") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetForwardVector().X, LeftController->MotionControllerComponent->GetForwardVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_Y") * PlayerController->LocomotionSpeed);
				AddMovementInput(FVector(LeftController->MotionControllerComponent->GetRightVector().X, LeftController->MotionControllerComponent->GetRightVector().Y, 0.0f), GetInputAxisValue("MotionControllerThumbRight_X") * PlayerController->LocomotionSpeed);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

// Called to bind functionality to input
void ABasePlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MotionControllerThumbRight_X", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MotionControllerThumbRight_Y", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MotionControllerThumbLeft_X", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MotionControllerThumbLeft_Y", this, &APawn::AddControllerPitchInput);

	if (!bUseCustomControls)
	{
		PlayerInputComponent->BindAction("TeleportLeft", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::TeleportLeft_Pressed);
		PlayerInputComponent->BindAction("TeleportLeft", EInputEvent::IE_Released, this, &ABasePlayerPawn::TeleportLeft_Released);
		PlayerInputComponent->BindAction("TeleportRight", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::TeleportRight_Pressed);
		PlayerInputComponent->BindAction("TeleportRight", EInputEvent::IE_Released, this, &ABasePlayerPawn::TeleportRight_Released);

		PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::GrabLeft_Pressed);
		PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Released, this, &ABasePlayerPawn::GrabLeft_Released);
		PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::GrabRight_Pressed);
		PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Released, this, &ABasePlayerPawn::GrabRight_Released);
	}
}

void ABasePlayerPawn::ExecuteTeleportation(ABaseMotionController* MC)
{
	if (!PlayerController->bIsTeleporting)
	{
		if (MC->bIsValidTeleportDestination)
		{
			PlayerController->bIsTeleporting = true;
			APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			CameraManager->StartCameraFade(0.0f, 1.0f, PlayerController->FadeDuration, FLinearColor(0, 0, 0, 1));

			TeleportMotionController = MC;
			GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ABasePlayerPawn::PostPlayerTeleportation, PlayerController->FadeDuration, false);

		}
		else
		{
			MC->DisableTeleporter();
		}
	}
}

void ABasePlayerPawn::UpdateActorLocation()
{
	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

	SetActorLocation(FVector(UKismetMathLibrary::Subtract_VectorVector(CapsuleComponent->GetComponentLocation(), FVector(HMDPos.X, HMDPos.Y, CapsuleComponent->GetUnscaledCapsuleHalfHeight()-10.0f))), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

TArray<AActor*> ABasePlayerPawn::GetAllActorsFromActor(AActor* Actor)
{
	TArray<AActor*> LocalActors;
	LocalActors.Add(Actor);

	for (size_t i = 0; i < LocalActors.Num(); i++)
	{
		TArray<AActor*> LocalActorArray;
		LocalActors[i]->GetAttachedActors(LocalActorArray);
		LocalActors.Append(LocalActorArray);
	}

	return LocalActors;
}

float ABasePlayerPawn::TwoPointTracking()
{
	return UKismetMathLibrary::FindLookAtRotation(CameraComp->GetComponentLocation(), UKismetMathLibrary::Divide_VectorFloat(UKismetMathLibrary::Add_VectorVector(LeftController->MotionControllerComponent->GetComponentLocation(), RightController->MotionControllerComponent->GetComponentLocation()), 2.0f)).Yaw;
}

void ABasePlayerPawn::IKFootTick()
{
	RightFoot = UKismetMathLibrary::FInterpTo(RightFoot, IKFootTrace("ik_foot_rSocket", FeetInverseKinematicsTraceDistance), UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) , InverseKinematicsInterpSpeed);
	LeftFoot = UKismetMathLibrary::FInterpTo(LeftFoot, IKFootTrace("ik_foot_lSocket", FeetInverseKinematicsTraceDistance), UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), InverseKinematicsInterpSpeed);
}

void ABasePlayerPawn::UpdateHandIK()
{
	BodyComp->SetWorldRotation(FRotator(0.0f, TwoPointTracking(), 0.0f));
}

ABaseMotionController* ABasePlayerPawn::SpawnMotionController(EControllerHand HandType)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.bDeferConstruction = true;

	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	if (!IsValid(PlayerController->HandControllerClass))
		UE_LOG(LogTemp, VeryVerbose, TEXT("FATAL: HandControllerClass is not valid!"));
		//ULog::Fatal("HandControllerClass is not valid!");

	ABaseMotionController* MC = GetWorld()->SpawnActor<ABaseMotionController>(PlayerController->HandControllerClass->GetDefaultObject()->GetClass(), SpawnParams);

	if (IsValid(MC))
	{
		MC->HandType = HandType;
		MC->SetInstigator(this);
		MC->FinishSpawning(FTransform(FRotator(), FVector(0.0f, 0.0f, 50.0f), FVector(1.0f, 1.0f, 1.0f)));
		MC->SetMCSource();
		MC->SetOwner(this);
		MC->AttachToComponent(VROriginComp, Rules);
	}
	else UE_LOG(LogTemp, VeryVerbose, TEXT("FATAL: Unable to spawn motion controller!"));

	return MC;
}

float ABasePlayerPawn::GetInverseKinematicFPS()
{
	return 1 / InverseKinematicsFPS;
}

float ABasePlayerPawn::IKFootTrace(FName SocketName, float TraceDistance)
{
	FVector StartLoc = FVector(BodyComp->GetSocketLocation(SocketName).X, BodyComp->GetSocketLocation(SocketName).Y, (CameraComp->GetComponentLocation().Z - 70.0f));
	FVector EndLoc = FVector(StartLoc.X, StartLoc.Y, (CameraComp->GetComponentLocation().Z - 70.0f - TraceDistance));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult;

	bool hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLoc, EndLoc, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, PlayerController->bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, HitResult, true);

	return UKismetMathLibrary::SelectFloat(UKismetMathLibrary::Subtract_VectorVector(EndLoc, HitResult.Location).Size(), 0.0f, hit);
}

void ABasePlayerPawn::UpdateRoomScalePosition()
{
	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);
	FVector CurrentRoomscaleLocation = FVector(HMDPos.X, HMDPos.Y, 0.0f);

	CapsuleComponent->AddWorldOffset(UKismetMathLibrary::Subtract_VectorVector(CurrentRoomscaleLocation, LastRoomscaleLocation), false, nullptr, ETeleportType::TeleportPhysics);
	LastRoomscaleLocation = CurrentRoomscaleLocation;
}

void ABasePlayerPawn::UpdateControllerTeleportRotation()
{
	if (IsValid(LeftController))
	{
		if (LeftController->bIsTeleporterActive)
		{
			LeftController->TeleportRotation = GetRotationFromInput(GetInputAxisValue("MotionControllerThumbLeft_Y"), GetInputAxisValue("MotionControllerThumbLeft_X"), LeftController);
		}
	}
	if (IsValid(RightController))
	{
		if (RightController->bIsTeleporterActive)
		{
			RightController->TeleportRotation = GetRotationFromInput(GetInputAxisValue("MotionControllerThumbRight_Y"), GetInputAxisValue("MotionControllerThumbRight_X"), RightController);
		}
	}
}

void ABasePlayerPawn::CheckCharacterPosition()
{
	TArray<AActor*> LocalActorsToIgnore;

	LocalActorsToIgnore = GetAllActorsFromActor(this);

	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

	FHitResult HitResult;

	float capsuleHH = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float capsuleRad = CapsuleComponent->GetScaledCapsuleRadius();

	FVector TestVec = UKismetMathLibrary::Add_VectorVector(GetActorLocation(), FVector(HMDPos.X, HMDPos.Y, capsuleHH));
	FVector TestVec2 = FVector(0.0f, 0.0f, capsuleHH - (capsuleRad + 3.0f));
	FVector StartVec = UKismetMathLibrary::Add_VectorVector(TestVec, TestVec2);
	FVector EndVec = UKismetMathLibrary::Subtract_VectorVector(TestVec, TestVec2);

	bool IsColliding = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartVec, EndVec, capsuleRad, ETraceTypeQuery::TraceTypeQuery1, true, LocalActorsToIgnore, PlayerController->bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None , HitResult, true);

	PlayerController->bCanTeleport = !IsColliding;

	if (IsColliding)
	{
		LeftController->DisableTeleporter();
		RightController->DisableTeleporter();
	}
	else
	{
		UpdateActorLocation();
	}
}

FRotator ABasePlayerPawn::GetRotationFromInput(float UpAxis, float RightAxis, ABaseMotionController* MC, float LocalDeadzone)
{
	FRotator OutputVal = FRotator(0.0f, 0.0f, 0.0f);

	if (PlayerController->bUseControllerRollToRotate)
	{
		FTransform Trans1 = FTransform(FRotator(MC->InitialControllerRotation), FVector(), FVector());
		FTransform Trans2 = FTransform(MC->MotionControllerComponent->GetComponentTransform());
		OutputVal.Yaw = UKismetMathLibrary::MakeRelativeTransform(Trans2, Trans1).GetRotation().X * 3 + GetActorRotation().Yaw;
	}
	else
	{
		if ((UKismetMathLibrary::Abs(UpAxis) + UKismetMathLibrary::Abs(RightAxis)) >= LocalDeadzone)
		{
			FVector Vec = FVector(UpAxis, RightAxis, 0.0f);
			Vec.Normalize();

			FQuat Rot = FQuat(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));

			OutputVal = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::Quat_RotateVector(Rot, Vec));
		}
		else OutputVal = GetActorRotation();
	}

	return OutputVal;
}

void ABasePlayerPawn::CapsuleComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABasePlayerPawn::CapsuleComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABasePlayerPawn::TeleportLeft_Pressed()
{
	StartPlayerMovement(LeftController);
}

void ABasePlayerPawn::TeleportRight_Pressed()
{
	StartPlayerMovement(RightController);
}

void ABasePlayerPawn::GrabLeft_Pressed()
{
	GrabObject(LeftController);
}

void ABasePlayerPawn::GrabRight_Pressed()
{
	GrabObject(RightController);
}

void ABasePlayerPawn::TeleportLeft_Released()
{
	StopPlayerMovement(LeftController);
}

void ABasePlayerPawn::TeleportRight_Released()
{
	StopPlayerMovement(RightController);
}

void ABasePlayerPawn::GrabLeft_Released()
{
	ReleaseObject(LeftController);
}

void ABasePlayerPawn::GrabRight_Released()
{
	ReleaseObject(RightController);
}

float ABasePlayerPawn::GetTriggerValue(ABaseMotionController* MC)
{
	float value = 0.0f;
	switch (MC->HandType)
	{
	case EControllerHand::Left:
		value = GetInputAxisValue(FName("ActivateLeft"));
		break;
	case EControllerHand::Right:
		value = GetInputAxisValue(FName("ActivateRight"));
		break;
	}
	return value;
}

//AVRPlayerController* ABasePlayerPawn::GetPlayerControllerRef()
//{
//	AVRPlayerController* PlayerController = PlayerController;
//	if (!IsValid(PlayerController)) ULog::Fatal("Player controller is not derived from AVRPlayerController or cannot access variable 'HandControllerClass'!");
//	return PlayerController;
//}
