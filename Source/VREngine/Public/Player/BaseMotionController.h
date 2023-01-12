// Copyright 2017-2022 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalStructs.h"
#include "BaseMotionController.generated.h"

class UArrowComponent;

UCLASS()
class VRENGINE_API ABaseMotionController : public AActor
{
	GENERATED_BODY(abstract)

public:
	// Sets default values for this actor's properties
	ABaseMotionController();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UPhysicsConstraintComponent* PhysicsConstraint;

	/* Component for the bound of the play area.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USteamVRChaperoneComponent* SteamVRChaperoneComp;

	/* Component for the direction of the teleport arc.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UArrowComponent* ArcDirectionComp;

	/* Component for teleporting.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* TeleportCylinderComp;

	/* Component for teleporting.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* RingComp;

	/* Static mesh component for the Direction of where the player is facing after teleportation.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* ArrowComp;

	/* Mesh component that shows the scale of the play area for teleportation.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* RoomScaleMeshComp;

	/* Spline component for teleport arc.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USplineComponent* ArcSplineComp;

	/* Mesh component that will appear at the end of the teleport.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* ArcEndPointComp;

	/* Sphere component for item detection.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USphereComponent* GrabSphereComponent;

	/* Sphere component Attach Location.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USphereComponent* AttachComponent;

	/* Motion controller component for getting the tracked controller location and rotation.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UMotionControllerComponent* MotionControllerComponent;

	/* Physics handler component for physics-based Item interaction.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UPhysicsHandleComponent* PhysicsHandler;

	/* Arrow component for the rotation of actor that are being held.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ItemRotationComponent;

	/* Default scene component.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USceneComponent* SceneComponent;

	/* Sphere component to show where the end of the hand is.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USphereComponent* WristComponent;

	/* Skeletal mesh component for the player's hand.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* HandMeshComponent;

	/* Skeletal mesh component to show player's hand while colliding with an object.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* GhostHandMeshComponent;

	/* Mesh for the spline for teleporting.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UStaticMesh* SplineMesh;

	/* Material for the spline for teleporting.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UMaterialInterface* SplineMaterial;

	/* Reference to player.*/
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	class ABasePlayerPawn* Player;

	/* Debug collision related things.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debugging")
		bool bCollisionDebugMode = false;

	/* True for if the player has set up a play area bounds.*/
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		bool bIsRoomScale;

	/* Frames per second of hand location updates, usually safe to keep this at 90.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
		float HandLocationUpdateFPS = 90;
	
	/* Velocity of the player's hand.*/
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
		float Velocity = 0;

	/* True if the player is currently teleporting.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Teleporting")
		bool bIsTeleporterActive = false;

	/* Initial Controller rotation for teleporting.*/
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Teleporting")
		FRotator InitialControllerRotation;

	/* Damage multiplier for player hitting objects.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float DamageMultiplier = 1.0f;

	/* True if the player wants to grab an object.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Input | Grabbing")
		bool bWantsToGrip = false;

	/* True if this is a second hand to an object.*/
	UPROPERTY(BlueprintReadOnly, Category = "Player Input | Grabbing")
		bool bIsSecondHand = false;

	/* True if this has grabbed an object.*/
	UPROPERTY(BlueprintReadOnly, Category = "Player Input | Grabbing")
		bool bHasGrabbedObject = false;

	/* Enables the visibility of a ghost mesh representing the player's hand through an object.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cosmetics")
		bool bUseGhostMesh = true;

	/* The reference to the currently attached Item. Null if no Item is attached.*/
	UPROPERTY(BlueprintReadOnly, Category = "Player Input")
		class AActor* AttachedItem;

	/* The reference to the Item we're currently hovering over. Null if we're currently not hovering over any.*/
	UPROPERTY(BlueprintReadOnly, Category = "Player Input")
		class AActor* HoveringItem;

	/* Which hand is for this controller, Valid Types are: Left and Right.*/
	UPROPERTY(BlueprintReadOnly, Category = "Setup")
		EControllerHand HandType = EControllerHand::Left;

	/* The haptic feedback used for when the player's hand goes through an object.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Effects")
		class UHapticFeedbackEffect_Curve* RumbleHapticEffect;

	/* Rotation of the Teleport.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement | Teleporting")
		FRotator TeleportRotation;

	/* What type of controller is this?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input | Controller")
		EMotionControllerType ControllerType;

	/* True if the controller found a valid destination it can teleport to.*/
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Teleporting")
		bool bIsValidTeleportDestination = false;

	/* Grabs the closest item in range.*/
	UFUNCTION(BlueprintCallable, Category = "Player Input | Grabbing")
		void GrabActor();

	/* Releases the item that this is currently holding.*/
	UFUNCTION(BlueprintCallable, Category = "Player Input | Grabbing")
		void ReleaseActor();

	/*
	Grab a specific actor.
	@param ActorToGrab - The actor you want to grab.
	@param InteractedComponent - the specific component you want to attach to.
	@return A bool value indicating if we were successfully able to grab the actor.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player Input")
		bool GrabSpecificActor(AActor* ActorToGrab, USceneComponent* InteractedComponent);

	///*
	//Gets the trigger value for this specific hand.
	//@return A float value between 0 - 1 for the trigger on the controller.
	//*/
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Input")
	//	float GetTriggerValue();

	/*
	Gets the closest item to the controller in range.
	@return Returns the closest item.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Collision")
		FNearestActorRet GetActorNearHand();

	/*
	makes the controller ruble.
	@param HapticEffect - The haptic effect to use.
	@param Intensity - How much the effect will be.
	@param bLoop - Bool value for if the effect will only be stopped if StopRumbleController is called.
	*/
	UFUNCTION(BlueprintCallable, Category = "Haptic Effects")
		void RumbleController(class UHapticFeedbackEffect_Curve* HapticEffect, float Intensity, bool bLoop);

	/* Stops the controller vibrating*/
	UFUNCTION(BlueprintCallable, Category = "Haptic Effects")
		void StopRumbleController();

	/* Sets the required variables for SteamVR.*/
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void SetMCSource();

	/*
	Gets the current location of the TeleportCylinderComp and rotation of TeleportRotation.
	@param Location - Variable used to store the return location.
	@param Rotation - Variable used to store the return Rotation.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement | Teleporting")
		void GetTeleportDestination(FVector &Location, FRotator &Rotation);

	/* Activates the teleporter for this controller.*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Teleporting")
		void ActivateTeleporter();

	/* Disables the teleporter for this controller.*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Teleporting")
		void DisableTeleporter();

	inline void UpdateHandPhysics() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void HandComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void GrabsphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void GrabsphereComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	inline void SetupRoomScaleOutline();

	inline void ClearArc();

	inline void UpdateArcSpline(bool FoundValidLocation, TArray<FVector> SplinePoints);

	inline void UpdateArcEndpoint(FVector NewLocation, bool HasValidLocation) const;

	inline void UpdateGhostHand();

	inline void DoOnce_HasGrabbedActorA_Func();

	inline void DoOnce_HasGrabbedActorB_Func();

	inline void DoOnce_HasGrabbedActorA_FuncReset();

	inline void DoOnce_HasGrabbedActorB_FuncReset();

	inline void UpdateRoomScaleOutline() const;

	inline bool TraceTeleportDest(TArray<FVector> &TracePoints, FVector &NavMeshLocation, FVector &TraceLocation);

	bool DoOnce_HasGrabbedActorA = false;

	bool DoOnce_HasGrabbedActorB = false;

	TArray<class USplineMeshComponent*> SplineMeshes;

	bool bLastFrameValidDestination = false;

	/* Position of the player's hand last frame tick, used to calculate the player's hand velocity.*/
	FVector LastFramePosition;

	/* This is so we can track the time for each hand update.*/
	float UpdateTimer = 0.0f;

	UPROPERTY()
		TArray<UPrimitiveComponent*> OverlappingComponents;

	UPROPERTY()
		class AVRPlayerController* PlayerController;

		EMotionControllerType GetControllerType();

};
