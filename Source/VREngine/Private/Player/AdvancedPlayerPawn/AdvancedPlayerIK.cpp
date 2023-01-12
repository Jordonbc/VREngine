// Copyright 2017-2022 Jordon Brooks.

#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AdvancedPlayerPawn.h"
#include "Player/VRPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogAdvancedPlayerInverseKinematics, All, All)

void AAdvancedPlayerPawn::CalculateMovement(const float DeltaTime)
{
	FTransform CurrentCameraPosition;

	CurrentCameraPosition.SetLocation(FVector(CameraComp->GetComponentLocation().X, CameraComp->GetComponentLocation().Y, 0.0f));
	CurrentCameraPosition.SetRotation(FRotator(0.0f, CameraComp->GetComponentRotation().Yaw, 0.0f).Quaternion());

	const float DistanceMoved = GET_DISTANCE_MOVED(CurrentCameraPosition.GetLocation().X, CurrentCameraPosition.GetLocation().Y);

	const float DistanceRotated = GET_DISTANCE_ROTATED(CurrentCameraPosition.GetRotation().Rotator());

	FVector BodyOffsetLoc;
	FRotator BodyOffsetRot;
	GetBodyOffset(BodyOffsetLoc, BodyOffsetRot);
	
	if (DistanceMoved > MovementThreshold || DistanceRotated > RotationThreshold)
	{
		//UE_LOG(LogAdvancedPlayerInverseKinematics, Verbose, TEXT("Setting new target!"))
		BodyTargetPosition.SetLocation(BodyOffsetLoc);
		BodyTargetPosition.SetRotation(BodyOffsetRot.Quaternion());
		MovementDirection = GetDirection(CurrentCameraPosition);
		LastCameraPosition = CurrentCameraPosition;
		MovementSpeed = (DistanceMoved + DistanceRotated / 2000) / DeltaTime;
	}
	else if (UKismetMathLibrary::EqualEqual_TransformTransform(BodyCurrentPosition, BodyTargetPosition))
	{
		//UE_LOG(LogAdvancedPlayerInverseKinematics, Verbose, TEXT("Looks Like we hit our goal!"))
		MovementSpeed = 0.0f;
		MovementDirection = 0.0f;
		Alpha = 0.0f;
	}
	else
	{
		//UE_LOG(LogAdvancedPlayerInverseKinematics, Verbose, TEXT("MovingPlayerBody!"))
		Alpha = UKismetMathLibrary::FInterpTo_Constant(Alpha, 1.0f, DeltaTime, MovementSpeed);
		BodyCurrentPosition = UKismetMathLibrary::TLerp(BodyCurrentPosition, BodyTargetPosition, Alpha);
		BodyComp->SetWorldLocationAndRotation(BodyCurrentPosition.GetLocation(), BodyCurrentPosition.GetRotation());
	}
	
	BodyComp->SetWorldLocationAndRotation(FVector(BodyCurrentPosition.GetLocation().X, BodyCurrentPosition.GetLocation().Y, BodyOffsetLoc.Z), BodyCurrentPosition.GetRotation());
}

void AAdvancedPlayerPawn::GetBodyOffset(FVector& Location, FRotator& Rotation) const
{
	const FRotator Rot = FRotator(0.0f, CameraComp->GetComponentRotation().Yaw - 90.0f, 0.0f);
	
	FVector Loc =
	CameraComp->GetComponentLocation() + UKismetMathLibrary::Multiply_VectorInt(
		UKismetMathLibrary::GetRightVector(Rot),-20.0f);

	Loc.Z = Loc.Z - PlayerHeight;

	Location = Loc;
	Rotation = Rot;
}

float AAdvancedPlayerPawn::GetDirection(FTransform CurrentCameraPosition) const
{
	const FVector NormalizedLastCameraPositionRotator = GET_NORMALIZED_ROTATOR_MO_Z(LastCameraPosition);

	const FVector NormalizedCurrentCameraPositionRotator = GET_NORMALIZED_ROTATOR_MO_Z(CurrentCameraPosition);

	const FVector NormalizedLastCameraPositionVector = GET_NORMALIZED_VECTOR_MO_Z(LastCameraPosition);
		
	const FVector NormalizedCurrentCameraPositionVector = GET_NORMALIZED_VECTOR_MO_Z(CurrentCameraPosition);

	const FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(
		((NormalizedLastCameraPositionRotator + NormalizedCurrentCameraPositionRotator) * 0.5f),
		(NormalizedLastCameraPositionVector - NormalizedCurrentCameraPositionVector)
		);

	const float DotProduct =
		UKismetMathLibrary::DegAcos(
	UKismetMathLibrary::Dot_VectorVector(
		((NormalizedLastCameraPositionRotator + NormalizedCurrentCameraPositionRotator) * 0.5f),
		NormalizedLastCameraPositionVector - NormalizedCurrentCameraPositionVector
		)) * ( CrossProduct.Z > 0 ? -1.0f : 1.0f);

	return DotProduct;
}
