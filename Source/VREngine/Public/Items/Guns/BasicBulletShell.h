// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "Items/StaticMeshItem.h"
#include "BasicBulletShell.generated.h"

UCLASS()
class VRENGINE_API ABasicBulletShell : public AStaticMeshItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicBulletShell();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundBase* HitSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnBulletComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void DeactivateProjectileMovementComponent();

	UPROPERTY()
	FTimerHandle TimerDeactivateProjectileMovementComponent;

	UPROPERTY()
		float HitTime1;
};
