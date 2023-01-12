// Copyright 2017-2022 Jordon Brooks.

#include "Player/BasePlayerPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/DrawSphereComponent.h"
#include "Player/BaseMotionController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/VRCamera.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Player/VRPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayer, All, All)

// Sets default values
ABasePlayerPawn::ABasePlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogPlayer, Verbose, TEXT("Creating BasePlayerPawn components"));

	DefaultSceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	VROriginComp = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	CameraComp = CreateDefaultSubobject<UVRCamera>(TEXT("Camera"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));

	LocomotionSphereComponent = CreateDefaultSubobject<UDrawSphereComponent>(TEXT("LocomotionSphere"));
	LocomotionSphereComponent->SetSimulatePhysics(true);
	
	VROriginComp->SetupAttachment(DefaultSceneRootComp);
	LocomotionSphereComponent->SetupAttachment(DefaultSceneRootComp);
	CapsuleComponent->SetupAttachment(DefaultSceneRootComp);
	CapsuleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 88.0f));
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetSimulatePhysics(false);

	//Workaround: SetMassOverrideInKg() fails when packaging
	CapsuleComponent->BodyInstance.bOverrideMass = true;
	CapsuleComponent->BodyInstance.SetMassOverride(62.0f);

	CameraComp->SetupAttachment(VROriginComp);
	
	SetRootComponent(DefaultSceneRootComp);
}

void ABasePlayerPawn::SetTrackingPoint() const
{
	if (IsValid(PlayerController))
	{
		const FString DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();

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
	}
}

// Called when the game starts or when spawned
void ABasePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AVRPlayerController>(GetController());
	
	SetTrackingPoint();
	
	if (IsValid(PlayerController))
	{
		if (PlayerController->MovementType == EMoveMode::Locomotion)
		{
			bUseControllerRotationYaw = true;
		}
		
		LeftController = SpawnMotionController(EControllerHand::Left);
		RightController = SpawnMotionController(EControllerHand::Right);
	}
}

// Called every frame
void ABasePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CapsuleComponent->SetWorldLocation(LocomotionSphereComponent->GetComponentLocation() + FVector(0.0f, 0.0f,
		CapsuleComponent->GetScaledCapsuleHalfHeight()));
	
	if (CameraComp->GetComponentLocation() != LastTickCameraLocation)
	{
		if (FVector::Dist(LastTickCameraLocation, CameraComp->GetComponentLocation()) > 0.05)
			PlayerVelocity = FVector::Dist(LastTickCameraLocation, CameraComp->GetComponentLocation());
		else
			PlayerVelocity = 0.0f;

		LastTickCameraLocation = CameraComp->GetComponentLocation();
	}
	
	UpdateRoomScalePosition();
	UpdateActorLocation();
}

// Called to bind functionality to input
void ABasePlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	const AVRPlayerController* PlayerController2 = Cast<AVRPlayerController>(GetController());
	
	switch (PlayerController2->MovementType)
	{
		case EMoveMode::Locomotion:
			PlayerInputComponent->BindAxis("MotionControllerThumbRight_X", this, &ABasePlayerPawn::RightMC_MoveLeftRight);
			PlayerInputComponent->BindAxis("MotionControllerThumbRight_Y", this, &ABasePlayerPawn::RightMC_MoveForwardBackwards);
			PlayerInputComponent->BindAxis("MotionControllerThumbLeft_X", this, &ABasePlayerPawn::LeftMC_MoveLeftRight);
			PlayerInputComponent->BindAxis("MotionControllerThumbLeft_Y", this, &ABasePlayerPawn::LeftMC_MoveForwardBackwards);
			break;
		case EMoveMode::LocomotionWithZ:
			PlayerInputComponent->BindAxis("MotionControllerThumbRight_X", this, &ABasePlayerPawn::RightMC_MoveLeftRight);
			PlayerInputComponent->BindAxis("MotionControllerThumbRight_Y", this, &ABasePlayerPawn::RightMC_MoveForwardBackwards);
			PlayerInputComponent->BindAxis("MotionControllerThumbLeft_X", this, &ABasePlayerPawn::LeftMC_MoveLeftRight);
			PlayerInputComponent->BindAxis("MotionControllerThumbLeft_Y", this, &ABasePlayerPawn::LeftMC_MoveForwardBackwards);
			break;
		case EMoveMode::Teleport:
			PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &ABasePlayerPawn::TeleportLeft_Pressed);
			PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &ABasePlayerPawn::TeleportLeft_Released);
			PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &ABasePlayerPawn::TeleportRight_Pressed);
			PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &ABasePlayerPawn::TeleportRight_Released);
			break;
		default:
			break;
	}

	if (!bUseCustomControls)
	{
		PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::GrabLeft_Pressed);
		PlayerInputComponent->BindAction("GrabLeft", EInputEvent::IE_Released, this, &ABasePlayerPawn::GrabLeft_Released);
		PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::GrabRight_Pressed);
		PlayerInputComponent->BindAction("GrabRight", EInputEvent::IE_Released, this, &ABasePlayerPawn::GrabRight_Released);

		PlayerInputComponent->BindAxis("ActivateRight", this, &ABasePlayerPawn::ActivateRight_Input);
		PlayerInputComponent->BindAxis("ActivateLeft", this, &ABasePlayerPawn::ActivateLeft_Input);

		PlayerInputComponent->BindAction("ActivateLeft", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::ActivateLeft_Pressed);
		PlayerInputComponent->BindAction("ActivateLeft", EInputEvent::IE_Released, this, &ABasePlayerPawn::ActivateLeft_Released);
		PlayerInputComponent->BindAction("ActivateRight", EInputEvent::IE_Pressed, this, &ABasePlayerPawn::ActivateRight_Pressed);
		PlayerInputComponent->BindAction("ActivateRight", EInputEvent::IE_Released, this, &ABasePlayerPawn::ActivateRight_Released);
	}
}

// There *must* be a better way of doing this!
TArray<AActor*> ABasePlayerPawn::GetAllActorsFromActor(AActor* Actor)
{
	TArray<AActor*> LocalActors;
	LocalActors.Add(Actor);

	UE_LOG(LogPlayer, VeryVerbose, TEXT("Getting all actors within actor!"));
	for (int i = 0; i < LocalActors.Num(); i++)
	{
		TArray<AActor*> LocalActorArray;
		LocalActors[i]->GetAttachedActors(LocalActorArray);
		LocalActors.Append(LocalActorArray);
	}

	return LocalActors;
}

ABaseMotionController* ABasePlayerPawn::SpawnMotionController(EControllerHand HandType)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.bDeferConstruction = true;

	const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	if (!IsValid(PlayerController->HandControllerClass) && PlayerController->HandControllerClass == nullptr)
		UE_LOG(LogPlayer, Fatal, TEXT("FATAL: HandControllerClass is not valid!"));

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
	else UE_LOG(LogPlayer, Fatal, TEXT("FATAL: Unable to spawn motion controller!"));

	return MC;
}

float ABasePlayerPawn::GetTriggerValue(ABaseMotionController* MC)
{
	float Value = 0.0f;
	switch (MC->HandType)
	{
	case EControllerHand::Left:
		Value = GetInputAxisValue(FName("ActivateLeft"));
		break;
	case EControllerHand::Right:
		Value = GetInputAxisValue(FName("ActivateRight"));
		break;
	default:
		break;
	}
	return Value;
}