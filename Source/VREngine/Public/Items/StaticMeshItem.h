// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicItem.h"
#include "StaticMeshItem.generated.h"

/**
 *
 */
UCLASS()
class VRENGINE_API AStaticMeshItem : public ABasicItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStaticMeshItem();

	class UStaticMeshComponent* GetStaticMeshComponent();

private:
	UPROPERTY()
	UStaticMeshComponent* RawStaticMeshComponent;
};
