// Copyright 2017-2020 Jordon Brooks.

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



    template<class T>
    static bool ArrayContainsClass(TArray<TSubclassOf<T>> ArrayOfClasses, UClass* OtherClass)
    {
        return ArrayOfClasses.ContainsByPredicate([OtherClass](TSubclassOf<T> Element) { return OtherClass->IsChildOf(Element); });
    }
};
