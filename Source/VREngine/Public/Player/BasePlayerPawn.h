// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GlobalEnums.h"
#include "BasePlayerPawn.generated.h"

UCLASS()
class VRENGINE_API ABasePlayerPawn : public APawn
{
	GENERATED_BODY(abstract)

public:
	ABasePlayerPawn();

	/* DefaultSceneRoot, probably not required but here anyways.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
		USceneComponent* DefaultSceneRootComp;

	/* Player controller reference, Used instead of having to cast every time I need to get controller settings. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerControllerContext")
	class AVRPlayerController* PlayerController;

	/* Center of the virtual reality play area.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USceneComponent* VROriginComp;

	/* Camera component so the player can see.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UCameraComponent* CameraComp;

	/* Spring arm component for the heads-up display (HUDComp).*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USpringArmComponent* SpringArmComp;

	/* Body skeletal mesh component.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* BodyComp;

	/* Head skeletal mesh component.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* HeadComp;

	/* Collision to check for if the player is colliding with an object.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UCapsuleComponent* CapsuleComponent;

	/* FloatingPawnMovement for locomotion.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* FloatingPawnMovement;

	/* The widget used for a heads-up display.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UWidgetComponent* HUDComp;

	/* The left controller actor.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Motion Controller")
		class ABaseMotionController* LeftController;

	/* The right controller actor.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Motion Controller")
		class ABaseMotionController* RightController;

	/* The distance in which how far teleport can go.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Teleporting")
		float TeleportLaunchVelocity = 900.0f;

	/* Frames per second of the inverse kinematics solvers, lower if you are experiencing lag.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inverse Kinematics")
		float InverseKinematicsFPS = 90;

	/* The distance for the feet inverse kinematics.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inverse Kinematics")
		float FeetInverseKinematicsTraceDistance = 100;

	/* The value of the right foot, used for inverse kinematics in blueprint animations.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inverse Kinematics")
		float RightFoot = 0;

	/* The value of the left foot, used for inverse kinematics in blueprint animations.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inverse Kinematics")
		float LeftFoot = 0;
	
	/* The interpolation speed of inverse kinematic (IK) functions.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inverse Kinematics")
		float InverseKinematicsInterpSpeed = 20;

	/* Use old tracking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inverse Kinematics")
		bool bUseOldTracking = true;

	/* If true the player will not use default button bindings, if false the player will ignore all default bindings to use your own. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller")
		bool bUseCustomControls = false;

	/*
	Activates the teleporter for a specified motion controller.
	@param MC - The motion controller you want to teleport with.
	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Teleporting")
		void ActivateTeleporter(class ABaseMotionController* MC);

	/* Moves the player using the specified MovementType.
	@param MC - The motion controller you want to move with.
	*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StartPlayerMovement(class ABaseMotionController* MC);

	/* Stops the player moving.*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void StopPlayerMovement(class ABaseMotionController* MC);

	/*
	Executes the teleporter for a specified motion controller.
	@param MC - The motion controller you want to teleport with.
	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Teleporting")
		void ExecuteTeleporter(class ABaseMotionController* MC);

	/*
	Check a motion controller to see what type it is.
	@param MC - The motion controller to check against.
	@param MCType - Type of controller you want to check for e.g. Valve Index or Oculus.
	@return a bool which indicates if the controller is the specified type.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsControllerType(class ABaseMotionController* MC, EMotionControllerType MCType);

	
	/*
	Makes the controller grab whatever is in front of it.
	@param MC - The motionController to use.
	*/
	UFUNCTION(BlueprintCallable)
		void GrabObject(class ABaseMotionController* MC);

	/*
	Makes the controller release it's currently grabbed object.
	@param MC - The controller to use.
	*/
	UFUNCTION(BlueprintCallable)
		void ReleaseObject(class ABaseMotionController* MC);

	UFUNCTION()
		void PostPlayerTeleportation();

	/*
	Turns the Specified input axis mappings into a rotation value.
	@param UpAxis - Axis that will be mapped to the pitch.
	@param RightAxis - Axis that will be mapped to the Yaw.
	@param MC - The motion controller to use if bUseControllerRollToRotate is true.
	@param Deadzone - Specifies how much the value has to be in order to return a value greater than 0.
	@return The rotation value of the UpAxis, RightAxis with the deadzone of Deadzone.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FRotator GetRotationFromInput(float UpAxis, float RightAxis, class ABaseMotionController* MC, float LocalDeadzone = 0.7f);


	/*
	
		INPUT PROCRSSING
	
	*/
	UFUNCTION()
		void CapsuleComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void CapsuleComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void TeleportLeft_Pressed();

	UFUNCTION()
		void TeleportRight_Pressed();

	UFUNCTION()
		void GrabLeft_Pressed();

	UFUNCTION()
		void GrabRight_Pressed();

	UFUNCTION()
		void TeleportLeft_Released();

	UFUNCTION()
		void TeleportRight_Released();

	UFUNCTION()
		void GrabLeft_Released();

	UFUNCTION()
		void GrabRight_Released();

	UFUNCTION()
		void ActivateLeft_Input(float Val);

	UFUNCTION()
		void ActivateRight_Input(float Val);

	UFUNCTION()
		void ActivateLeft_Pressed();

	UFUNCTION()
		void ActivateRight_Pressed();

	UFUNCTION()
		void ActivateRight_Released();

	UFUNCTION()
		void ActivateLeft_Released();

	UFUNCTION()
		void ActivateObject(class ABaseMotionController* MC);

	UFUNCTION()
		void DeactivateObject(class ABaseMotionController* MC);

	/*
	Get's the trigger value for this specific hand.
	@param MC - The controller you wish to use.
	@return A float value between 0 - 1 for the trigger on the controller.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Input")
		float GetTriggerValue(ABaseMotionController* MC);

protected:
	virtual void BeginPlay() override;

	/*
	Executes the "ExecuteTeleportation" function for a given motion controller.
	@param MC - The controller you wish to use.
	*/
	UFUNCTION(BlueprintCallable)
		void ExecuteTeleportation(class ABaseMotionController* MC);

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
		/*
		* Updates actor's position to CapsuleComponent world location.
		*/
		inline void UpdateActorLocation();

		/*
		Gets All the actors attached to an actor.
		@param Actor - Actor to search.
		*/

		inline TArray<class AActor*> GetAllActorsFromActor(AActor* Actor);

		/* This uses the camera and  both motion controllers to approximate the general player direction. Easy to implement but doesn't work very well. */
		inline float TwoPointTracking();

		/* This function basically line traces the players feet so the feet always stay on the ground and knees bend.
		This works really well but can be somewhat expensive on lower end systems.
		*/
		inline void IKFootTick();

		/* This function is responsible for updating the players arms to always be connected to the hand.
		This has to be done since there is no way to map SteamVR controller skeleton to a UE4 skeleton, so we just use a second actor and point the arms in the direction.
		*/
		inline void UpdateHandIK();

		/* This spawns the player hands and is called once for each hand. */
		inline class ABaseMotionController* SpawnMotionController(EControllerHand HandType);

		/*
		Function used to specifically return the value of the delay needed to execute inverse kinematic (IK) calculations.
		@return - Float value containing timing data specifically used for IK calculations.
		*/
		inline float GetInverseKinematicFPS();

		/*
		Line traces from a bone on the Z axis to the TraceDistance.
		@param SocketName - Name of the bone or socket you would like to query.
		@param TraceDistance - Distance the line trace should go before giving up.
		@returns Float value containing the distance from the start point a hit was found.
		*/
		inline float IKFootTrace(FName SocketName, float TraceDistance);


		/* Updates the box around the player often called the chaperone */
		inline void UpdateRoomScalePosition();

		// TODO [$5fefbd554e7350000773fe22]: Find out why this function is not working
		// The player cannot change teleport direction with thumbsticks.
		/* Update the player teleport direction. */
		inline void UpdateControllerTeleportRotation();

		/*
		Checks to see if anything is colliding with the character and disables movement.
		*/
		inline void CheckCharacterPosition();


		// I still don't know if this runs on a different thread to speed up tick. None of these need to happen on tick thread but still need to be updated.

		/* Timer function handler for feetIK.*/
		FTimerHandle IKFootTickFunc;

		/* Timer function handler for hand IK.*/
		FTimerHandle UpdateHandIKFunc;

		/* Timer function for teleporting.*/
		FTimerHandle TeleportTimerHandle;

		/* Temporary variable to pass the motion controller from pre-teleport to post-teleport.*/
		class ABaseMotionController* TeleportMotionController;

		/* Last location of head mounted display without Z axis.*/
		FVector LastRoomscaleLocation;
};
