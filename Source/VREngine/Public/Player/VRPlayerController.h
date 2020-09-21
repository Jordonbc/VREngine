// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GlobalEnums.h"
#include "VRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VRENGINE_API AVRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AVRPlayerController();

	/* Class to use to spawn the controllers if using a custom blueprint class.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller")
	TSubclassOf<class ABaseMotionController> HandControllerClass;

	/* The Default height of the player in centimeters in the real world.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Information")
		float DefaultPlayerHeight = 180.0f;

	/* Should only be true if using PSVR.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Teleporting")
		bool bUseControllerRollToRotate = false;

	/* Change how to handle controller collision*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		EControllerPhysicsType ControllerPhysics = EControllerPhysicsType::Stop;

	/* True if the player is currently teleporting.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Teleporting")
		bool bIsTeleporting = false;

	/* True if the player is allowed to teleport, usually returns false for collisions.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Teleporting")
		bool bCanTeleport = false;

	/* The movement mode the player will use.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		EMoveMode MovementType = EMoveMode::Teleport;

	/* The direction mode of locomotion.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Locomotion")
		EMoveDirection MovementDirection = EMoveDirection::Camera;

	/* The direction mode of locomotion.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		EControllerSelection ControllerMovementInput = EControllerSelection::Left;

	/* Fade Duration.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Teleporting")
		float FadeDuration = 0.1;

	/* Locomotion Speed.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Locomotion")
		float LocomotionSpeed = 0.5f;

	/* Debug collision related things.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debugging")
		bool bCollisionDebugMode = false;

	/* Deadzone*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
		float ThumbstickDeadzone = 0.5f;
};
