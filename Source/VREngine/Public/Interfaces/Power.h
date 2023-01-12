// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Power.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UPower : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class VRENGINE_API IPower
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power")
		void OnHasPower(float Power);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power")
		void OnLostPower();
};