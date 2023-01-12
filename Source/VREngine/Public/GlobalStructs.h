#pragma once
#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GlobalEnums.h"
#include "GlobalStructs.generated.h"

/**
*
*/


USTRUCT(BlueprintType)
struct FNearestActorRet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nearest Actor Return")
		class AActor* NearestActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nearest Actor Return")
		class UPrimitiveComponent* InteractedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nearest Actor Return")
		float DistanceToActor = 10000.0f;
};