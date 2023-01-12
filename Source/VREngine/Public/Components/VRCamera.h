// 

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "VRCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VRENGINE_API UVRCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVRCamera();

	UPROPERTY()
		float UpdateSpeed = 90.0f;

	UPROPERTY()
		bool bIsServer;

	UPROPERTY()
		bool bHasAuthority = false;

	UPROPERTY()
	bool bSmoothMotion;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TickUpdateTracking(float DeltaTime);

	UFUNCTION()
	void ServerReplicateTracking(FTransform TrackingData);

	UFUNCTION(Server, Reliable)
	void Net_RPC_ClientSendTracking(FTransform TrackingData);

private:
	UPROPERTY()
		FTransform LastPosition;
};
