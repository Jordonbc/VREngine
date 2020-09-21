// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Player/BaseMotionController.h"
#include "UObject/Interface.h"
#include "VRDualHands.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVRDualHands : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class VRENGINE_API IVRDualHands
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_Pickup(USceneComponent* MotionController, ABaseMotionController* MotionControllerActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_Drop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		bool SecondHand_IsHeldByMe(UMotionControllerComponent* MotionController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_Activate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_Deactivate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_SpecialActivate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_SpecialDeactivate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_HoverBegin();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		void SecondHand_HoverEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		bool HasSecondHand();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR DualHands")
		bool HasFirstHand();
};
