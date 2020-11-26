// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticMeshItem.h"
#include "BasicSword.generated.h"

/**
 * 
 */
UCLASS()
class VRENGINE_API ABasicSword : public AStaticMeshItem
{
	GENERATED_BODY()

public:
	ABasicSword();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
		class UBoxComponent* Handle;

protected:
	UFUNCTION()
		void OnSwordHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
