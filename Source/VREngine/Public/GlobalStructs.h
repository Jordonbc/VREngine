#pragma once
#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GlobalEnums.h"
#include "GlobalStructs.generated.h"

/**
*
*/

UCLASS()
class VRENGINE_API UGlobalStructs : public UUserDefinedStruct

{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FNearestActorRet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor* NearestActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPrimitiveComponent* InteractedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DistanceToActor = 10000.0f;
};