// 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseVRAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRENGINE_API UBaseVRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
		class AAdvancedPlayerPawn* Player;
	

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void FootIK();
	void TraceGround(FVector FootLocation, FVector& HitLocation, FRotator& AlignedRotation, float& FootDistanceToGround) const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FRotator FootRotationL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FRotator FootRotationR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	float FootIKEffectorL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	float FootIKEffectorR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	float Direction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FTransform PlayerCameraTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FTransform LeftMotionControllerTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FTransform RightMotionControllerTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FVector JointLocationL = FVector(-16.0f, 1000.0f, -16.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base VR Animation")
	FVector JointLocationR = FVector(-17.0f, 27.0f, 15.0f);
	

private:
	float DTime;
	UPROPERTY()
		APawn* Pawn;
};
