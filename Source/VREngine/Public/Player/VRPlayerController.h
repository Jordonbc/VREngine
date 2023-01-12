// Copyright 2017-2022 Jordon Brooks.

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

	/* Change how to handle controller collision*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		EControllerPhysicsType ControllerPhysics = EControllerPhysicsType::PhysicsConstraint;

	/* True if the player is currently teleporting.*/
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		bool bIsTeleporting = false;

	/* The movement mode the player will use.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMoveMode MovementType = EMoveMode::Teleport;
	
	/* Should only be true if using PSVR.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Teleport", EditConditionHides))
	bool bUseControllerRollToRotate = false;

	/* True if the player is allowed to teleport, usually returns false for collisions.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Teleport", EditConditionHides))
		bool bCanTeleport = true;
	
	/* Fade Duration.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Teleport", EditConditionHides))
	float FadeDuration = 0.1;

	/* Deadzone*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Locomotion", EditConditionHides))
	float ThumbstickDeadzone = 0.5f;

	/* The direction mode of locomotion.  (Controller Rotation is currently broken)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Locomotion", EditConditionHides))
		EMoveDirection MovementDirection = EMoveDirection::Camera;

	/* The direction mode of locomotion.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Locomotion", EditConditionHides))
		EControllerSelection ControllerMovementInput = EControllerSelection::Left;

	/* Locomotion Speed.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="MovementType == EMoveMode::Locomotion", EditConditionHides))
		float LocomotionSpeed = 0.5f;

	/* Debug collision related things.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debugging")
		bool bCollisionDebugMode = false;
};
