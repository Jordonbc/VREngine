// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/VRDualHands.h"
#include "Interfaces/Damage.h"
#include "BasicItem.generated.h"

UCLASS(abstract)
class VRENGINE_API ABasicItem : public AActor, public IInteractionInterface, public IDamage
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasicItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR Interfaces")
		bool bIsGrabbed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		float Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		FVector Velocity3D;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageMultiplier = 1;

	UPROPERTY()
		FVector LastFramePosition;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class ABaseMotionController* MotionControllerBP;

	UPROPERTY(EditAnywhere)
		EGrabType GrabMethod = EGrabType::Mixed;

	UPROPERTY(VisibleAnywhere)
		EGrabType CurrentGrabMethod = GrabMethod;

	UPROPERTY(EditAnywhere)
		float DropDistance = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachToCenter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bActivateOnlyCenter = true;

	/* Array of actors to exclude from physics interactions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> ActorsToIgnore;

	/* Array of actors to exclude from physics interactions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Interactions")
		TArray<TSubclassOf<AActor>> ClassesToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Interfaces")
		TArray<UPrimitiveComponent*> MyComponents;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UPrimitiveComponent* MainMesh;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void Pickup(class ABaseMotionController* MotionControllerActor, bool Center);
		virtual void Pickup_Implementation(class ABaseMotionController* MotionControllerActor, bool Center) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void Drop();
		virtual void Drop_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		bool IsHeldByMe(ABaseMotionController* MotionControllerActor);
		virtual bool IsHeldByMe_Implementation(ABaseMotionController* MotionControllerActor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void Activate();
		virtual void Activate_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void Deactivate();
		virtual void Deactivate_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void SpecialActivate();
		virtual void SpecialActivate_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void SpecialDeactivate();
		virtual void SpecialDeactivate_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void HoverBegin(ABaseMotionController* MotionControllerActor);
		virtual void HoverBegin_Implementation(ABaseMotionController* MotionControllerActor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void HoverEnd(ABaseMotionController* MotionControllerActor);
		virtual void HoverEnd_Implementation(ABaseMotionController* MotionControllerActor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void HoverTrigger(ABaseMotionController* MotionControllerActor);
		virtual void HoverTrigger_Implementation(ABaseMotionController* MotionControllerActor);

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		void DropAttachTo();

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		void DropPhysicsHandle();

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		void DropMixedMode();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Collision")
		void RecieveDamage(float DamageAmount, FHitResult HitResult);
	void RecieveDamage_Implementation(float DamageAmount, FHitResult HitResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

	UFUNCTION(Category = "VR Interfaces")
		virtual void SetupParams();

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		void AttachBeforeTp();

	UFUNCTION(BlueprintNativeEvent)
		void OnActivate();
		virtual void OnActivate_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void OnDeactivate();
		virtual void OnDeactivate_Implementation();

	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UFUNCTION()
		bool ValidateHit(AActor* Actor);

	UFUNCTION()
		void Reset_DoOnce_Physics();

	UFUNCTION()
		void Reset_DoOnce_AttachTo();

	UFUNCTION()
		void SwitchToPhysics();

	UFUNCTION()
		void SwitchToAttachTo();

	UFUNCTION()
		void DoOnce_ToPhysics();

	UFUNCTION()
		void DoOnce_ToAttachTo();

	UPROPERTY()
		bool bDoOnce_Physics = false;

	UPROPERTY()
		float HitTime = 0;

	UPROPERTY()
		bool bDoOnce_AttachedTo = true;

	UPROPERTY(VisibleAnywhere, Category = "VR Interfaces")
		FTransform RelativePickupOffset;

	UPROPERTY(VisibleAnywhere, Category = "VR Interfaces")
		float LocationAlpha;

	UFUNCTION()
		void DoTransform();

	UFUNCTION()
		bool SelfHitTest();

	UFUNCTION()
		bool CheckHandDistance(float MaxDistance);

	UFUNCTION()
		void HandleMixedPhysics();

	UFUNCTION()
		void GrabAttachTo();

	UFUNCTION()
		void GrabPhysicsHandle();

	UFUNCTION()
		void GrabMixedMode();

	UFUNCTION()
		FTransform GetWorldPickupTransform();

	UFUNCTION()
		FTransform GetRelativePickupTransform();

	UFUNCTION()
		void OpenDoGates();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UPrimitiveComponent*> DetectedOverlaps;
};
