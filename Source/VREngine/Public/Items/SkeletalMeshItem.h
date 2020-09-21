// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicItem.h"
#include "SkeletalMeshItem.generated.h"

/**
 *
 */
UCLASS()
class VRENGINE_API ASkeletalMeshItem : public ABasicItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletalMeshItem();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
