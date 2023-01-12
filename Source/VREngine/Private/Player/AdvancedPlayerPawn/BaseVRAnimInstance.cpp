// 

#include "Player/BaseVRAnimInstance.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Player/AdvancedPlayerPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/BaseMotionController.h"

void UBaseVRAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	if (IsValid(TryGetPawnOwner()))
	{
		Pawn = TryGetPawnOwner();
		Player = Cast<AAdvancedPlayerPawn>(Pawn);
	}
	
}

void UBaseVRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	DTime = DeltaSeconds;

	if (IsValid(Player))
	{
		Player = Cast<AAdvancedPlayerPawn>(Pawn);
		
		FootIK();

		Speed = Player->MovementSpeed;
		Direction = Player->MovementDirection;
		PlayerCameraTransform = Player->CameraComp->GetComponentTransform();
		
		if (IsValid(Player->LeftController))
			LeftMotionControllerTransform = Player->LeftController->WristComponent->GetComponentTransform();
		
		if (IsValid(Player->RightController))
			RightMotionControllerTransform = Player->RightController->WristComponent->GetComponentTransform();
	}
}

void UBaseVRAnimInstance::FootIK()
{
	const FVector LeftFootSocket = Player->BodyComp->GetSocketLocation(FName("foot_l"));
	const FVector RightFootSocket = Player->BodyComp->GetSocketLocation(FName("foot_r"));

	FVector LeftHitLocation;
	FRotator LeftAlignedRotator;
	float LeftFootDistanceToGround;
	TraceGround(FVector(LeftFootSocket.X, LeftFootSocket.Y,Player->BodyComp->GetComponentLocation().Z), LeftHitLocation, LeftAlignedRotator, LeftFootDistanceToGround);

	FootRotationL = LeftAlignedRotator;
	FootIKEffectorL = LeftFootDistanceToGround;

	
	FVector RightHitLocation;
	FRotator RightAlignedRotator;
	float RightFootDistanceToGround;
	TraceGround(FVector(RightFootSocket.X, RightFootSocket.Y,Player->BodyComp->GetComponentLocation().Z), RightHitLocation, RightAlignedRotator, RightFootDistanceToGround);

	FootRotationR = RightAlignedRotator;
	FootIKEffectorR = RightFootDistanceToGround;
}

void UBaseVRAnimInstance::TraceGround(FVector FootLocation, FVector& HitLocation, FRotator& AlignedRotation, float& FootDistanceToGround) const
{
	FHitResult Hit;
	const bool bHasHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		FVector(FootLocation.X, FootLocation.Y, FootLocation.Z + 200.0f),
		FVector(FootLocation.X, FootLocation.Y, FootLocation.Z - 200.0f),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Hit,
		true);
	
	if ( bHasHit && Hit.Location.Z > FootLocation.Z)
	{
		//HitLocation = Hit.ImpactPoint;
		HitLocation = Hit.Location;
		AlignedRotation = FRotator(atan2(Hit.Normal.Y, Hit.Normal.Z), 0.0f, atan2(Hit.Normal.X, Hit.Normal.Z));
		FootDistanceToGround = abs(Hit.Location.Z - FootLocation.Z);
		return;
	}

	HitLocation = FVector::ZeroVector;
	AlignedRotation = FRotator::ZeroRotator;
	FootDistanceToGround = 0.0f;
}
