// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/SkeletalMeshItem.h"
#include "Components/TimelineComponent.h"
#include "UObject/UnrealType.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "BasicGun.generated.h"

/**
 *
 */


UCLASS(abstract)
class VRENGINE_API ABasicGun : public ASkeletalMeshItem, public IVRDualHands
{
	GENERATED_BODY()

public:
	ABasicGun();
	virtual void OnActivate_Implementation() override;
	virtual void OnDeactivate_Implementation() override;
	virtual void SetupParams() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float RoundsPerMinute = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float Range = 350;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float ShellSize = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		bool bSpawnShells = true;

	UPROPERTY(BlueprintReadOnly, Category = "Gun Stats")
		bool bCanFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		int32 FireEffectProbability = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		TSubclassOf<class ABasicBulletShell> EmptyBulletShellClass;

	/* This is the bullets that are in the chamber */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		uint8 InternalAmmo = 0;

	/* This is the maximum amount of ammo that can go in to the chamber - shotguns will be > 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		uint8 MaxInternalAmmo = 1;

	/* This is the value that will drive the trigger animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float TriggerValue = 0.0f;

	/* This is the value that drives the reloading part */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun Stats")
		float ReloadValue;

	/* The normalized value of ReloadValue where 0 offset is 0 and max offset is 1 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun Stats")
		float ReloadValueNormalized;

	/* If True, the ammo will be consumed like a shotgun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		bool bConsumeAmmo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		bool bLineTraced = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		TSubclassOf<class ABasicPhysicalBullet> PhysicalBulletClass;

	/* This is the amount of damage the gun deals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float Damage = 10.0f;

	/* This is the force that is applied to object that are shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float ForceAppliedToObjects = 100.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		float CooldownSpeed = 20.0f;

	/* Firing mode of the gun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		EFireMode FireMode = EFireMode::DoubleAction;

	/* Sound played when the gun is empty */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class USoundBase* EmptySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class USoundBase* ReloadSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class USoundBase* ReloadSound2;

	/* Sound played when the gun hits something */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class USoundBase* HitSound;

	/* Sound played when the gun fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Audio")
		class UParticleSystem* FiringPS;

	/* True if the ammo collision component has collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision | Bools")
		bool HasAmmoCollision = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision | Bools")
		bool bHasReloadingPart = true;

	/* Array of the different ammo types allowed for this gun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Stats")
		TArray<TSubclassOf<class ABasicAmmo>> CompatibleAmmoTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attached Actors")
		TArray<FAttachment> Attachments;

	/* Component Handle of the gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class UBoxComponent* GunHandle;

	/* Component responsible for detecting ammo */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class UBoxComponent* AmmoCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class ABaseMotionController* SecondHandMotionController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class UBoxComponent* ReloadCollisionBox;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class USphereComponent* ShootSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class USphereComponent* BulletShellSpawn;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class USphereComponent* MaxReload;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		class USphereComponent* ReloadEnd;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
		class ABaseMotionController* ReloadMotionController;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void Shoot();
	virtual void Shoot_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void PreShoot();
	virtual void PreShoot_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Interfaces")
		float GetRPM();

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		bool PreInitAttachment(class ABasicAttachment* Attachment);

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		bool AttachAttachment(ABasicAttachment* Attachment, FName SocketName = FName(""));

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		bool DetachAttachment(FAttachment AttachmentToDetach);

	UFUNCTION(BlueprintCallable, Category = "VR Interfaces")
		bool DetachItem(class ABasicAttachment* Attachment);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void OnAmmoAttached();
	virtual void OnAmmoAttached_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void OnAmmoRemoved();
	virtual void OnAmmoRemoved_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void SecondHand_Pickup(USceneComponent* MotionController, ABaseMotionController* MotionControllerActor);
	virtual void SecondHand_Pickup_Implementation(USceneComponent* MotionController, ABaseMotionController* MotionControllerActor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void SecondHand_Drop();
	virtual void SecondHand_Drop_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		bool SecondHand_IsHeldByMe(UMotionControllerComponent* MotionController);
	virtual bool SecondHand_IsHeldByMe_Implementation(UMotionControllerComponent* MotionController) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void EjectShell();
	virtual void EjectShell_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void OnEmpty();
	virtual void OnEmpty_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Interfaces")
		void ShootAnimation(bool IsEmpty);
	virtual void ShootAnimation_Implementation(bool IsEmpty);

	UFUNCTION()
		FAttachment GetAmmoAttachment();

	UFUNCTION()
		void EmptyAnimationCurve_Update(float value);

	UFUNCTION()
		void FullReloadAnimationCurve_Update(float value);

private:
	UPROPERTY()
		bool bFiring = false;
	UPROPERTY()
		float Timer;

	UPROPERTY()
		bool bAutomaticFire = false;

	UPROPERTY()
		class UTimelineComponent* EmptyAnimationCurve;

	FOnTimelineFloat EmptyAnimationCurveProgress {};

	UPROPERTY()
		class UTimelineComponent* FullReloadAnimationCurve;

	UPROPERTY()
		FOnTimelineFloat FullReloadAnimationCurveProgress;

	FOnTimelineEvent FullReloadAnimation_StartEvent {};

	FOnTimelineEvent FullReloadAnimation_EndEvent{};

	UFUNCTION()
		void FullReloadAnimation_Start();

	UFUNCTION()
		void FullReloadAnimation_End();

	UFUNCTION()
		bool FillInternalAmmo();

	UFUNCTION()
		void ShootTick();

	UFUNCTION()
		void OnAmmoBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnReloadCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnReloadCollisionEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		float GetDistanceOfHand();

	UFUNCTION()
		float GetMaxReloadDistance();

	UFUNCTION()
		bool PullbackDistance();

	UFUNCTION()
		void DOONCE_HASRELOADED();
	
	UPROPERTY()
		bool DOONCE_HASRELOADED_BOOL;

	UFUNCTION()
		void DOONCE_HASRELOADED_RESET();

	UFUNCTION()
		void DOONCE_ISRELOADING();

	UPROPERTY()
		bool DOONCE_ISRELOADING_BOOL = true;

	UFUNCTION()
		void DOONCE_ISRELOADING_RESET();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

};
