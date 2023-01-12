// Copyright 2017-2022 Jordon Brooks.

#include "Player/BaseMotionController.h"

#include "Player/BasePlayerPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogMotionControllerTeleport, All, All)

bool ABaseMotionController::TraceTeleportDest(TArray<FVector>& TracePoints, FVector& NavMeshLocation, FVector& TraceLocation)
{
	TracePoints.Empty();

	float ProjectNavExtends = 500.0f;
	FVector ProjectedLocation;

	FPredictProjectilePathParams Params = FPredictProjectilePathParams();
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	Params.bTraceComplex = true;
	Params.StartLocation = ArcDirectionComp->GetComponentLocation();
	Params.LaunchVelocity = UKismetMathLibrary::Multiply_VectorFloat(ArcDirectionComp->GetForwardVector(), Player->TeleportLaunchVelocity);
	Params.bTraceWithCollision = true;
	Params.DrawDebugType = bCollisionDebugMode ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	Params.ActorsToIgnore.Add(this);
	Params.ActorsToIgnore.Add((AActor*)Player);


	FPredictProjectilePathResult Result = FPredictProjectilePathResult();

	bool Success1 = UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);

	for (int i = 0; i < Result.PathData.Num(); i++)
	{
		TracePoints.Add(Result.PathData[i].Location);
	}


	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);


	TSubclassOf<UNavigationQueryFilter> FilterClass;

	bool Success2 = UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), Result.HitResult.Location, ProjectedLocation, NavData, FilterClass, FVector(ProjectNavExtends));

	NavMeshLocation = ProjectedLocation;
	TraceLocation = Result.HitResult.Location;

	bool Success = UKismetMathLibrary::BooleanAND(Success1, Success2);

	UE_LOG(LogMotionControllerTeleport, VeryVerbose, TEXT("Teleport collision is %s"), Success ? TEXT("True") : TEXT("False"));

	return Success;
}

void ABaseMotionController::UpdateArcEndpoint(FVector NewLocation, bool HasValidLocation) const
{
	ArcEndPointComp->SetVisibility(UKismetMathLibrary::BooleanAND(HasValidLocation, bIsTeleporterActive), false);
	ArcEndPointComp->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	FVector HMDPos;
	FRotator HMDRot;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRot, HMDPos);
	ArcEndPointComp->SetWorldRotation(UKismetMathLibrary::ComposeRotators(TeleportRotation, FRotator(0.0f, HMDRot.Yaw, 0.0f)), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);

	RoomScaleMeshComp->SetWorldRotation(TeleportRotation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void ABaseMotionController::UpdateArcSpline(bool FoundValidLocation, TArray<FVector> SplinePoints)
{
	if (!FoundValidLocation)
	{
		SplinePoints.Empty();
		SplinePoints.Add(ArcDirectionComp->GetComponentLocation());
		SplinePoints.Add(ArcDirectionComp->GetComponentLocation() + (ArcDirectionComp->GetForwardVector() * 20));
	}
	for (FVector Point : SplinePoints)
	{
		ArcSplineComp->AddSplinePoint(Point, ESplineCoordinateSpace::Local, true);
	}

	ArcSplineComp->SetSplinePointType(SplinePoints.Num(), ESplinePointType::CurveClamped, true);

	for (int i = 0; i < (ArcSplineComp->GetNumberOfSplinePoints() - (int32)2); i++)
	{
		USplineMeshComponent* smc = NewObject<USplineMeshComponent>(this);
		smc->SetStaticMesh(SplineMesh);
		smc->SetMobility(EComponentMobility::Movable);
		smc->SetMaterial(0, SplineMaterial);
		smc->RegisterComponent();
		smc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		smc->SetStartAndEnd(SplinePoints[i], ArcSplineComp->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local), SplinePoints[i + 1], ArcSplineComp->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local), true);

		SplineMeshes.Add(smc);
	}
}

void ABaseMotionController::ActivateTeleporter()
{
	bIsTeleporterActive = true;
	TeleportCylinderComp->SetVisibility(true, true);
	RoomScaleMeshComp->SetVisibility(bIsRoomScale, false);
	InitialControllerRotation = MotionControllerComponent->GetComponentRotation();
}

void ABaseMotionController::DisableTeleporter()
{
	if (bIsTeleporterActive)
	{
		bIsTeleporterActive = false;
		TeleportCylinderComp->SetVisibility(false, true);
		ArcEndPointComp->SetVisibility(false, false);
		RoomScaleMeshComp->SetVisibility(false, false);
	}
}

void ABaseMotionController::ClearArc()
{
	for (USplineMeshComponent* SplineMeshComp : SplineMeshes)
	{
		SplineMeshComp->DestroyComponent();
	}
	SplineMeshes.Empty();
	ArcSplineComp->ClearSplinePoints();
}

void ABaseMotionController::GetTeleportDestination(FVector& Location, FRotator& Rotation)
{
	Location = TeleportCylinderComp->GetComponentLocation();

	Rotation = TeleportRotation;
}