// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicPhysicalBullet.generated.h"

UCLASS()
class VRENGINE_API ABasicPhysicalBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicPhysicalBullet();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Gun Audio")
		class ABasicGun* GunReference;

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
		void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
