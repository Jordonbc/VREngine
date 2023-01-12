// 

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerPawn.h"
#include "AdvancedPlayerPawn.generated.h"

#define GET_DISTANCE_MOVED(CurrentX, CurrentY) (UKismetMathLibrary::Vector_Distance(FVector(CurrentX, CurrentY, 0.0f), FVector(LastCameraPosition.GetLocation().X, LastCameraPosition.GetLocation().Y, 0.0f)))
#define GET_DISTANCE_ROTATED(CurrentRotation) (UKismetMathLibrary::Abs(CurrentRotation.Yaw - LastCameraPosition.GetRotation().Rotator().Yaw))
#define GET_NORMALIZED_ROTATOR_MO_Z(Transform) (UKismetMathLibrary::Normal( FVector(UKismetMathLibrary::GetForwardVector(Transform.Rotator()).X, UKismetMathLibrary::GetForwardVector(Transform.Rotator()).Y, 0.0f)))
#define GET_NORMALIZED_VECTOR_MO_Z(Transform) (UKismetMathLibrary::Normal(FVector(Transform.GetLocation().X, Transform.GetLocation().Y, 0.0f)))
#define GET_BODY_ROT_FROM_HMD() (FRotator(0.0f, CameraComp->GetComponentRotation().Yaw - 90.0f, 0.0f))

UCLASS()
class VRENGINE_API AAdvancedPlayerPawn : public ABasePlayerPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAdvancedPlayerPawn();

	/* Body skeletal mesh component.*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* BodyComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Advanced Player")
	float MovementThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Advanced Player")
	float RotationThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Advanced Player")
	float PlayerHeight = 180.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Advanced Player")
	float MovementDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Advanced Player")
	float MovementSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FTransform LastCameraPosition;
	float Alpha;
	FTransform BodyTargetPosition;
	FTransform BodyCurrentPosition;

	void CalculateMovement(const float DeltaTime);
	void GetBodyOffset(FVector& Location, FRotator& Rotation) const;
	float GetDirection(FTransform CurrentCameraPosition) const;
};
