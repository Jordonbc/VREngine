// Copyright 2017-2022 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GlobalFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VRENGINE_API UGlobalFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Global Function Library")
        static FName GetMotionControllerDeviceTypeName(class UMotionControllerComponent* MotionControllerComponent);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Global Function Library")
        static bool IsEditorSimulating();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Global Function Library", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
        static class AVRPlayerController* GetPlayerController(UWorld* WorldContextObject);
    /*
    Check a motion controller to see what type it is.
    @param MC - The motion controller to check against.
    @param MCType - Type of controller you want to check for e.g. Valve Index or Oculus.
    @return a bool which indicates if the controller is the specified type.
    */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Global Function Library")
    static bool IsControllerType(const class ABaseMotionController* MC, const enum EMotionControllerType MCType);


    template<class T>
    static bool ArrayContainsClass(TArray<TSubclassOf<T>> ArrayOfClasses, UClass* OtherClass)
    {
        return ArrayOfClasses.ContainsByPredicate([OtherClass](TSubclassOf<T> Element) { return OtherClass->IsChildOf(Element); });
    }
};
