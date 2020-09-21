// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class VRENGINE_API IInteractionInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void Pickup(class ABaseMotionController* MotionControllerActor, bool Center);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void Drop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		bool IsHeldByMe(ABaseMotionController* MotionControllerActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void Activate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void Deactivate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void HoverBegin(ABaseMotionController* MotionControllerActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VR Interactions")
		void HoverEnd(ABaseMotionController* MotionControllerActor);
};