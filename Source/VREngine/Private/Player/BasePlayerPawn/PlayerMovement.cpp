// Copyright 2017-2022 Jordon Brooks.

#include "DrawDebugHelpers.h"
#include "Player/BasePlayerPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/VRPlayerController.h"
#include "Player/BaseMotionController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/DrawSphereComponent.h"
#include "TimerManager.h"
#include "Components/VRCamera.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerMovement, All, All)

void ABasePlayerPawn::TeleportLeft_Pressed()
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Pressed Left Teleport"))
	StartPlayerMovement(LeftController);
}

void ABasePlayerPawn::TeleportRight_Pressed()
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Pressed Right Teleport"))
	StartPlayerMovement(RightController);
}

void ABasePlayerPawn::TeleportLeft_Released()
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Released Left Teleport"))
	StopPlayerMovement(LeftController);
}

void ABasePlayerPawn::TeleportRight_Released()
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Released Right Teleport"))
	StopPlayerMovement(RightController);
}

void ABasePlayerPawn::StartPlayerMovement(class ABaseMotionController* MC)
{
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
	UE_LOG(LogPlayerMovement, Verbose, TEXT("StopPlayerMovement"));
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

void ABasePlayerPawn::ActivateTeleporter(ABaseMotionController* MC)
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Activating Teleporter"))

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

void ABasePlayerPawn::ExecuteTeleporter(ABaseMotionController* MC)
{
	UE_LOG(LogPlayerMovement, Verbose, TEXT("Executing Teleporter"))
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
	//CapsuleComponent->SetWorldLocationAndRotation(UKismetMathLibrary::Add_VectorVector(DestLoc, FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight())),
	//																					DestRot, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	LocomotionSphereComponent->SetWorldLocationAndRotation(DestLoc, DestRot);
	
	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	CameraManager->StartCameraFade(0.0f, 1.0f, PlayerController->FadeDuration, FLinearColor(0, 0, 0, 1));
	PlayerController->bIsTeleporting = false;
	UpdateActorLocation();
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

			// Updates HandMeshComponent after a teleport
			switch (PlayerController->ControllerPhysics)
			{
			case EControllerPhysicsType::PhysicsConstraint:
				MC->UpdateHandPhysics();
				break;
			default:
				break;
			}

			TeleportMotionController = MC;
			GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ABasePlayerPawn::PostPlayerTeleportation, PlayerController->FadeDuration, false);
		}
		else
		{
			MC->DisableTeleporter();
		}
	}
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

void ABasePlayerPawn::LeftMC_MoveForwardBackwards(float AxisValue)
{
	if (AxisValue < PlayerController->ThumbstickDeadzone && AxisValue >(PlayerController->ThumbstickDeadzone * -1))
		return;
	
	if (PlayerController->ControllerMovementInput == EControllerSelection::Left || PlayerController->ControllerMovementInput == EControllerSelection::Both)
	{
		LocomotionSphereComponent->AddImpulse(UKismetMathLibrary::NegateVector(CameraComp->GetForwardVector()) * PlayerController->LocomotionSpeed * AxisValue);
	}
}

void ABasePlayerPawn::LeftMC_MoveLeftRight(float AxisValue)
{
	if (AxisValue < PlayerController->ThumbstickDeadzone && AxisValue >(PlayerController->ThumbstickDeadzone * -1))
		return;
	if (PlayerController->ControllerMovementInput == EControllerSelection::Left || PlayerController->ControllerMovementInput == EControllerSelection::Both)
	{
		LocomotionSphereComponent->AddImpulse((CameraComp->GetRightVector() * PlayerController->LocomotionSpeed) * AxisValue);
	}
}

void ABasePlayerPawn::RightMC_MoveForwardBackwards(float AxisValue)
{
	if (AxisValue < PlayerController->ThumbstickDeadzone && AxisValue >(PlayerController->ThumbstickDeadzone * -1))
		return;
	if (PlayerController->ControllerMovementInput == EControllerSelection::Right || PlayerController->ControllerMovementInput == EControllerSelection::Both)
	{
		LocomotionSphereComponent->AddImpulse(UKismetMathLibrary::NegateVector(CameraComp->GetForwardVector()) * PlayerController->LocomotionSpeed * AxisValue);
	}
}

void ABasePlayerPawn::RightMC_MoveLeftRight(float AxisValue)
{
	if (AxisValue < PlayerController->ThumbstickDeadzone && AxisValue >(PlayerController->ThumbstickDeadzone * -1))
		return;
	if (PlayerController->ControllerMovementInput == EControllerSelection::Right || PlayerController->ControllerMovementInput == EControllerSelection::Both)
	{
		LocomotionSphereComponent->AddImpulse((CameraComp->GetRightVector() * PlayerController->LocomotionSpeed) * AxisValue);
	}
}

void ABasePlayerPawn::CheckCharacterPosition()
{
	if (PlayerController->MovementType == EMoveMode::Teleport)
	{
		const TArray<AActor*> LocalActorsToIgnore = GetAllActorsFromActor(this);

		FVector HMDPos;
		FRotator HMDRot;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

		FHitResult HitResult;

		const float capsuleHH = CapsuleComponent->GetScaledCapsuleHalfHeight();
		const float capsuleRad = CapsuleComponent->GetScaledCapsuleRadius();

		const FVector TestVec = UKismetMathLibrary::Add_VectorVector(GetActorLocation(), FVector(HMDPos.X, HMDPos.Y, capsuleHH));
		const FVector TestVec2 = FVector(0.0f, 0.0f, capsuleHH - (capsuleRad + 3.0f));
		const FVector StartVec = UKismetMathLibrary::Add_VectorVector(TestVec, TestVec2);
		const FVector EndVec = UKismetMathLibrary::Subtract_VectorVector(TestVec, TestVec2);

		const bool IsColliding = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartVec, EndVec, capsuleRad, ETraceTypeQuery::TraceTypeQuery1, true, LocalActorsToIgnore, PlayerController->bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None , HitResult, true);

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
	else
		UpdateActorLocation();
}

FRotator ABasePlayerPawn::GetRotationFromInput(float UpAxis, float RightAxis, ABaseMotionController* MC, float LocalDeadzone)
{
	FRotator OutputVal = FRotator(0.0f, 0.0f, 0.0f);

	if (PlayerController->bUseControllerRollToRotate)
	{
		const FTransform Trans1 = FTransform(FRotator(MC->InitialControllerRotation), FVector(), FVector());
		const FTransform Trans2 = FTransform(MC->MotionControllerComponent->GetComponentTransform());
		OutputVal.Yaw = UKismetMathLibrary::MakeRelativeTransform(Trans2, Trans1).GetRotation().X * 3 + GetActorRotation().Yaw;
	}
	else
	{
		if ((UKismetMathLibrary::Abs(UpAxis) + UKismetMathLibrary::Abs(RightAxis)) >= LocalDeadzone)
		{
			FVector Vec = FVector(UpAxis, RightAxis, 0.0f);
			Vec.Normalize();

			const FQuat Rot = FQuat(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));

			OutputVal = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::Quat_RotateVector(Rot, Vec));
		}
		else OutputVal = GetActorRotation();
	}

	return OutputVal;
}

void ABasePlayerPawn::UpdateRoomScalePosition()
{
	//FVector CameraOffset = CameraComp->GetComponentLocation() - GetActorLocation();
	//AddActorWorldOffset(CameraOffset);
	//VROriginComp->AddWorldOffset(UKismetMathLibrary::NegateVector(CameraOffset));

	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

	const FVector CurrentRoomScaleLocation = FVector(HMDPos.X, HMDPos.Y, 0.0f);

	LocomotionSphereComponent->AddWorldOffset(CurrentRoomScaleLocation - LastRoomscaleLocation);

	LastRoomscaleLocation = CurrentRoomScaleLocation;
}

void ABasePlayerPawn::UpdateActorLocation()
{
	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);

	if (PlayerController->MovementType == EMoveMode::Teleport)
	{
		
	}
	SetActorLocation(UKismetMathLibrary::Subtract_VectorVector(LocomotionSphereComponent->GetComponentLocation(), FVector(HMDPos.X, HMDPos.Y, 0.0f)), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}