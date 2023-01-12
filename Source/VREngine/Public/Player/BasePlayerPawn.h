// Copyright 2017-2022 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePlayerPawn.generated.h"



UCLASS()
class VRENGINE_API ABasePlayerPawn : public APawn
{
	GENERATED_BODY(abstract)

public:
	ABasePlayerPawn();

	/* DefaultSceneRoot, Actor Root.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
		USceneComponent* DefaultSceneRootComp;

	/* Player controller reference, Used instead of having to cast every time I need to get controller settings. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerControllerContext")
		class AVRPlayerController* PlayerController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "LocomotionSphereComponent")
		class UDrawSphereComponent* LocomotionSphereComponent;

	/* Center of the virtual reality play area.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USceneComponent* VROriginComp;

	/* Camera component so the player can see.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UVRCamera* CameraComp;

	/* Collision to check for if the player is colliding with an object.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UCapsuleComponent* CapsuleComponent;

	/* FloatingPawnMovement for locomotion.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* FloatingPawnMovement;

	/* The left controller actor.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Motion Controller")
		class ABaseMotionController* LeftController;

	/* The right controller actor.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Motion Controller")
		class ABaseMotionController* RightController;

	/* The distance in which how far teleport can go.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Teleporting")
		float TeleportLaunchVelocity = 900.0f;

	/* The value of the right foot, used for inverse kinematics in blueprint animations.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inverse Kinematics")
		float RightFoot = 0;

	/* The value of the left foot, used for inverse kinematics in blueprint animations.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inverse Kinematics")
		float LeftFoot = 0;

	/* If true the player will not use default button bindings, if false the player will ignore all default bindings to use your own. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller")
		bool bUseCustomControls = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Info")
	float PlayerVelocity;

	/*
	Makes the controller grab whatever is in front of it.
	@param MC - The motionController to use.
	*/
	UFUNCTION(BlueprintCallable, Category = "VR Base Player")
		void GrabObject(class ABaseMotionController* MC);

	/*
	Makes the controller release it's currently grabbed object.
	@param MC - The controller to use.
	*/
	UFUNCTION(BlueprintCallable, Category = "VR Base Player")
		void ReleaseObject(class ABaseMotionController* MC) const;

	UFUNCTION()
		void ActivateObject(class ABaseMotionController* MC) const;

	UFUNCTION()
		void DeactivateObject(class ABaseMotionController* MC) const;

	/*
	Gets the trigger value for this specific hand.
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
	UFUNCTION(BlueprintCallable, Category = "VR Base Player")
		void ExecuteTeleportation(class ABaseMotionController* MC);

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base Player")
		FRotator GetRotationFromInput(float UpAxis, float RightAxis, class ABaseMotionController* MC, float LocalDeadzone = 0.7f);

	UFUNCTION()
	void SetTrackingPoint() const;


	/*
	
	INPUT PROCESSING
	
	*/

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

	UFUNCTION(BlueprintCallable, Category = "Movement | Locomotion")
		void LeftMC_MoveForwardBackwards(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Movement | Locomotion")
		void LeftMC_MoveLeftRight(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Movement | Locomotion")
		void RightMC_MoveForwardBackwards(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Movement | Locomotion")
		void RightMC_MoveLeftRight(float AxisValue);

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/* Temporary variable to pass the motion controller from pre-teleport to post-teleport.*/
	UPROPERTY()
		class ABaseMotionController* TeleportMotionController;

	/* Last location of head mounted display without Z axis.*/
	FVector LastRoomscaleLocation;

	FVector LastTickCameraLocation;
	
	/*
	* Updates actor's position to CapsuleComponent world location.
	*/
	inline void UpdateActorLocation();

	/*
	Gets All the actors attached to an actor.
	@param Actor - Actor to search.
	*/

	static inline TArray<class AActor*> GetAllActorsFromActor(AActor* Actor);

	/* This spawns the player hands and is called once for each hand. */
	inline class ABaseMotionController* SpawnMotionController(EControllerHand HandType);

	/* Updates the box around the player often called the chaperone */
	inline void UpdateRoomScalePosition();

	/* Update the player teleport direction. */
	inline void UpdateControllerTeleportRotation();

	/* Checks to see if anything is colliding with the character and disables movement.*/
	inline void CheckCharacterPosition();

	/* Timer function for teleporting.*/
	FTimerHandle TeleportTimerHandle;
};
