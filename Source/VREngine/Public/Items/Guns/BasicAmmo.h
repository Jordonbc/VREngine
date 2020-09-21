// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicAttachment.h"
#include "UObject/UnrealType.h"
#include "BasicAmmo.generated.h"

/**
 * 
 */
UCLASS(abstract)
class VRENGINE_API ABasicAmmo : public ABasicAttachment
{
	GENERATED_BODY()

public:
	ABasicAmmo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Stats")
		int32 MaxAmmoCapacity = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Stats")
		int32 CurrentAmmo = MaxAmmoCapacity;

};
