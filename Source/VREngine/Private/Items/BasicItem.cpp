// Copyright 2017-2022 Jordon Brooks.
#include "Items/BasicItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasicItem, All, All)

// Sets default values
ABasicItem::ABasicItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABasicItem::Pickup_Implementation(ABaseMotionController* MotionControllerActor, bool Center)
{
	if (IsValid(MotionControllerActor))
	{
		if (IsValid(MotionControllerBP)) Drop();
		MotionControllerBP = MotionControllerActor;
		bAttachToCenter = Center;
		ActorsToIgnore.Add(MotionControllerBP);

		switch (GrabMethod)
		{
		case EGrabType::AttachTo:
			GrabAttachTo();
			break;
		case EGrabType::Physics:
			GrabPhysicsHandle();
			break;
		case EGrabType::Mixed:
			GrabMixedMode();
			break;
		}
	}
}

void ABasicItem::Drop_Implementation()
{
	UE_LOG(LogTemp, Verbose, TEXT("Revieved Drop command!"));
	if (IsValid(MotionControllerBP))
	{
		MotionControllerBP->StopRumbleController();
		Deactivate();

		switch (GrabMethod)
		{
		case EGrabType::AttachTo:
			DropAttachTo();
			break;
		case EGrabType::Physics:
			DropPhysicsHandle();
			break;
		case EGrabType::Mixed:
			DropMixedMode();
		}
		MotionControllerBP->PhysicsHandler->SetLinearStiffness(5000.0f);
		MotionControllerBP->PhysicsHandler->SetAngularStiffness(5000.0f);

		ActorsToIgnore.Remove(MotionControllerBP);

		MotionControllerBP = nullptr;
	}
}

bool ABasicItem::IsHeldByMe_Implementation(ABaseMotionController* MotionControllerActor)
{
	if (IsValid(MotionControllerBP))
	{
		if (IsValid(MotionControllerActor))
		{
			if (MotionControllerActor->MotionControllerComponent == MotionControllerBP->MotionControllerComponent) return true;
			else return false;
		}
		else return false;
	}
	else return false;
}

void ABasicItem::Activate_Implementation()
{
	if (bIsGrabbed)
	{
		if (bActivateOnlyCenter && bAttachToCenter)
		{
			OnActivate();
		}
		else if (!bActivateOnlyCenter)
		{
			OnActivate();
		}
	}
}

void ABasicItem::Deactivate_Implementation()
{
	if (bIsGrabbed)
	{
		if (bActivateOnlyCenter && bAttachToCenter)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Deactivated!"));
			OnDeactivate();
		}
		else if (!bActivateOnlyCenter)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Deactivated!"));
			OnDeactivate();
		}
	}
}

void ABasicItem::SpecialActivate_Implementation()
{
	//SI_LOG("SpecialActivate");
}

void ABasicItem::SpecialDeactivate_Implementation()
{
	//SI_LOG("SpecialDeactivate");
}

void ABasicItem::HoverBegin_Implementation(ABaseMotionController* MotionControllerActor)
{
	//SI_LOG("Hovering Begin!!");
}

void ABasicItem::HoverEnd_Implementation(ABaseMotionController* MotionControllerActor)
{
	//SI_LOG("Hovering End");
}

void ABasicItem::HoverTrigger_Implementation(ABaseMotionController* MotionControllerActor)
{
	//SI_LOG("Hover Trigger");
}

void ABasicItem::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/*if (ValidateHit(OtherActor))
	{
		if (ABaseMotionController* mc = Cast<ABaseMotionController>(OtherActor))
			return;
		else
			HitTime = GetWorld()->GetTimeSeconds();

		UPrimitiveComponent* Comp = (UPrimitiveComponent*)OtherActor->GetRootComponent();

		if (Comp) Comp->SetMaterial(0, MainMesh->GetMaterial(0));
	}*/

	if (IsValid(OtherActor))
	{
		if (OtherActor->GetClass()->ImplementsInterface(UDamage::StaticClass()))
		{
			const float damage = FMath::Abs(Velocity) * DamageMultiplier;
			IDamage::Execute_RecieveDamage(OtherActor, damage, Hit);
			//IDamage::Execute_RecieveDamage(this, damage, Hit);
		}
	}
}

// Called when the game starts or when spawned
void ABasicItem::BeginPlay()
{
	Super::BeginPlay();

	const TSubclassOf<ABaseMotionController>M;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), M, ActorsToIgnore);


	MyComponents.Empty();
	TArray<UPrimitiveComponent*> TempArray;
	GetComponents<UPrimitiveComponent>(TempArray, true);
	MyComponents.Append(TempArray);
}

void ABasicItem::PostInitProperties()
{
	Super::PostInitProperties();
}

void ABasicItem::SetupParams()
{
	SetRootComponent(MainMesh);
	MainMesh->ComponentTags.Add(FName("Grip"));
	MainMesh->SetCollisionProfileName(FName("PhysicsActor"));
	MainMesh->CanCharacterStepUpOn = ECB_No;
	//MainMesh->ComponentTags.Add(FName("Grip"));
	MainMesh->SetSimulatePhysics(true);
	MainMesh->SetGenerateOverlapEvents(true);
	MainMesh->SetNotifyRigidBodyCollision(true);

	//AttachToLocation->SetupAttachment(MainMesh);

	MainMesh->OnComponentHit.AddDynamic(this, &ABasicItem::OnComponentHit);
}

void ABasicItem::AttachBeforeTp()
{
	Reset_DoOnce_AttachTo();
	DoOnce_ToAttachTo();
}

void ABasicItem::OnActivate_Implementation()
{
	//SI_LOG("Activating!");
}

void ABasicItem::OnDeactivate_Implementation()
{
	//SI_LOG("Deactivating");
}

bool ABasicItem::ValidateHit(AActor* Actor)
{
	bool bValid = true;

	for (int i = 0; i < ActorsToIgnore.Num(); i++)
	{
		if (ActorsToIgnore[i] == Actor)
		{
			bValid = false;
		}
	}
	return bValid;
}

void ABasicItem::Reset_DoOnce_Physics()
{
	bDoOnce_Physics = true;
}

void ABasicItem::Reset_DoOnce_AttachTo()
{
	bDoOnce_AttachedTo = true;
}

void ABasicItem::SwitchToPhysics()
{
	UE_LOG(LogTemp, Verbose, TEXT("Switching to physics mode"));
	//ULog::DebugMessage(EDebugLogType::DL_Info, FString("Switching to physics mode"));
	DropAttachTo();
	GrabPhysicsHandle();
}

void ABasicItem::SwitchToAttachTo()
{
	//ULog::DebugMessage(EDebugLogType::DL_Info, FString("Switching to attachto mode"));
	DropPhysicsHandle();
	GrabAttachTo();
}

void ABasicItem::DoOnce_ToPhysics()
{
	if (bDoOnce_Physics)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Switching to physics mode - Do Once"));
		//ULog::DebugMessage(EDebugLogType::DL_Info, FString("Switching to physics mode - Do Once"));
		bDoOnce_Physics = false;
		Reset_DoOnce_AttachTo();
		SwitchToPhysics();
	}
}

void ABasicItem::DoOnce_ToAttachTo()
{
	if (bDoOnce_AttachedTo)
	{
		//ULog::DebugMessage(EDebugLogType::DL_Info, FString("Switching to attachto mode - Do Once"));
		bDoOnce_AttachedTo = false;
		Reset_DoOnce_Physics();
		SwitchToAttachTo();
	}
}

void ABasicItem::DoTransform()
{
	LocationAlpha = LocationAlpha + (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 10);
	/*if (LocationAlpha > 1) LocationAlpha = 1;
	else if (LocationAlpha < 0) LocationAlpha = 0;*/

	//UE_LOG(LogBasicItem, VeryVerbose, TEXT("Location Alpha: %s"), LocationAlpha);

	FTransform NewTransform;
	FTransform CurrentTransform = MainMesh->GetComponentTransform();
	FTransform ToTransform = GetWorldPickupTransform();

	NewTransform = UKismetMathLibrary::TLerp(CurrentTransform, ToTransform, LocationAlpha, ELerpInterpolationMode::QuatInterp);
	//NewTransform.SetLocation(FMath::Lerp(CurrentTransform.GetLocation(), ToTransform.GetLocation(), LocationAlpha));
	//NewTransform.SetRotation(FQuat::Slerp(CurrentTransform.GetRotation(), ToTransform.GetRotation(), LocationAlpha));

	MainMesh->SetWorldLocationAndRotation(NewTransform.GetLocation(), NewTransform.GetRotation(), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

bool ABasicItem::SelfHitTest()
{
	//ULog::Info("SelfHitTest");
	TArray<TEnumAsByte <EObjectTypeQuery>> ObjectType;

	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_Destructible));
	ObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));

	//TArray<UPrimitiveComponent*> Overlaps;

	DetectedOverlaps.Empty();

	UKismetSystemLibrary::ComponentOverlapComponents(MainMesh, MainMesh->GetComponentTransform(), ObjectType, TSubclassOf<UPrimitiveComponent>(), ActorsToIgnore, DetectedOverlaps);

	// Remove all components that are a part of itself
	for (int i = 0; i < MyComponents.Num(); i++)
	{
		DetectedOverlaps.Remove(MyComponents[i]);
	}

	if (DetectedOverlaps.Num() > 0 && ClassesToIgnore.Num() > 0)
	{
		for (int a = DetectedOverlaps.Num()-1; a >= 0; a--)
		{
			for (int b = 0; b < ClassesToIgnore.Num(); b++)
			{
				if (DetectedOverlaps[a]->GetOwner()->GetClass()->IsChildOf(ClassesToIgnore[b]))
				{
					DetectedOverlaps.RemoveAt(a);
					break;
				}
			}
		}
	}

	if (DetectedOverlaps.Contains(MotionControllerBP->HandMeshComponent))
		DetectedOverlaps.Remove(MotionControllerBP->HandMeshComponent);

	if (DetectedOverlaps.Contains(MotionControllerBP->GhostHandMeshComponent))
		DetectedOverlaps.Remove(MotionControllerBP->GhostHandMeshComponent);

	if (DetectedOverlaps.Contains(MotionControllerBP->GrabSphereComponent))
		DetectedOverlaps.Remove(MotionControllerBP->GrabSphereComponent);

	if (DetectedOverlaps.Num() > 0)
	{
		HitTime = GetWorld()->GetTimeSeconds();
		return true;
	}
	else return false;
}

bool ABasicItem::CheckHandDistance(float MaxDistance) const
{
	if (IsValid(MotionControllerBP->GrabSphereComponent))
	{
		const FVector Vector = FVector(MainMesh->GetComponentLocation() - MotionControllerBP->GrabSphereComponent->GetComponentLocation());
		if (Vector.Size() > MaxDistance)
		{
			return true;
		}
	}
	return false;
}

void ABasicItem::HandleMixedPhysics()
{
	if (CheckHandDistance(DropDistance))
	{
		Drop();
	}
	else
	{
		if (IsValid(MotionControllerBP))
		{
			MotionControllerBP->PhysicsHandler->SetTargetLocationAndRotation(MotionControllerBP->GrabSphereComponent->GetComponentLocation(), MotionControllerBP->GrabSphereComponent->GetComponentRotation());

			if (MyComponents.Num() > 0) // Only run if we have at least 1 mesh
			{
				//UE_LOG(LogTemp, Verbose, TEXT("SelfHitTest() = %s"), a ? TEXT("True") : TEXT("False"));

				if (SelfHitTest())
				{
					DoOnce_ToPhysics();
				}

				else if (!((UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - HitTime) < 1.0f))
				{
					if (LocationAlpha > 1.0f)
					{
						DoOnce_ToAttachTo();
					}
					else
					{
						IVRDualHands* TheInterface = Cast<IVRDualHands>(this);
						if (TheInterface)
						{
							if (!TheInterface->Execute_HasSecondHand(this))
							{
								DoTransform();
							}
						}
						else
						{
							DoTransform();
						}
					}
				}
			}
		}
	}
}

void ABasicItem::GrabAttachTo()
{
	//ULog::Info("GrabAttachTo()");
	//ULog::DebugMessage(EDebugLogType::DL_Info, FName("GrabAttachTo()"));
	MainMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	MainMesh->SetEnableGravity(false);
	CurrentGrabMethod = EGrabType::AttachTo;

	if (bIsGrabbed)
	{
		const FTransform Temp = GetWorldPickupTransform();

		MainMesh->SetWorldLocationAndRotation(Temp.GetLocation(), Temp.GetRotation(), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	}
	MainMesh->SetSimulatePhysics(false);
	if (bAttachToCenter) MainMesh->AttachToComponent(MotionControllerBP->AttachComponent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld, true));

	else GetRootComponent()->AttachToComponent(MotionControllerBP->AttachComponent,
		FAttachmentTransformRules(EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepWorld, true));

	if (!bIsGrabbed) RelativePickupOffset = GetRelativePickupTransform();
	//GrabMethod = EGrabType::AttachTo;
}

void ABasicItem::GrabPhysicsHandle()
{
	MotionControllerBP->PhysicsHandler->GrabComponentAtLocationWithRotation(MainMesh, FName(""), MotionControllerBP->GrabSphereComponent->GetComponentLocation(), MotionControllerBP->GrabSphereComponent->GetComponentRotation());
	MainMesh->SetEnableGravity(false);
	MainMesh->SetSimulatePhysics(true);
	MainMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	LocationAlpha = 0.0f;
	CurrentGrabMethod = EGrabType::Physics;
	const float PhysicsMass = MainMesh->GetMass() * 10.0f;
	MotionControllerBP->PhysicsHandler->SetLinearDamping(PhysicsMass);
	MotionControllerBP->PhysicsHandler->SetAngularDamping(PhysicsMass);
}

void ABasicItem::GrabMixedMode()
{
	//ULog::DebugMessage(EDebugLogType::DL_Info, FName("GrabMixedMode()"));
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	GrabAttachTo();
	bIsGrabbed = true;
	OpenDoGates();
}

void ABasicItem::DropAttachTo()
{
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));
	//DetachRootComponentFromParent(true);
	//UE_LOG(LogTemp, Warning, TEXT("DROPPED!!!"));
	//ULog::DebugMessage(EDebugLogType::DL_Info, FName("DropAttachTo()"));
	MainMesh->SetSimulatePhysics(true);
	MainMesh->SetEnableGravity(true);

	MainMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABasicItem::DropPhysicsHandle()
{
	if (IsValid(MotionControllerBP->PhysicsHandler))
	{
		//ULog::DebugMessage(EDebugLogType::DL_Info, FName("DropPhysicsHandle()"));
		MotionControllerBP->PhysicsHandler->ReleaseComponent();
		MainMesh->SetEnableGravity(true);
	}
}

void ABasicItem::DropMixedMode()
{
	//ULog::DebugMessage(EDebugLogType::DL_Info, FName("DropMixedMode()"));
	bIsGrabbed = false;
	DropPhysicsHandle();
	DropAttachTo();
	MainMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	MainMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	LocationAlpha = 0.0f;
}

void ABasicItem::RecieveDamage_Implementation(float DamageAmount, FHitResult HitResult)
{
	// What happens when this object takes damage
}

FTransform ABasicItem::GetWorldPickupTransform() const
{
	FTransform NewTransform = FTransform(MotionControllerBP->HandMeshComponent->GetComponentTransform());
	NewTransform.SetLocation(NewTransform.TransformPosition(RelativePickupOffset.GetLocation()));
	NewTransform.SetRotation(NewTransform.TransformRotation(RelativePickupOffset.GetRotation()));

	return NewTransform;
}

FTransform ABasicItem::GetRelativePickupTransform() const
{
	FTransform NewTransform = FTransform(MotionControllerBP->HandMeshComponent->GetComponentTransform());
	NewTransform.SetLocation(NewTransform.InverseTransformPosition(GetActorLocation()));
	NewTransform.SetRotation(NewTransform.InverseTransformRotation(GetActorRotation().Quaternion()));

	return NewTransform;
}

void ABasicItem::OpenDoGates()
{
	Reset_DoOnce_AttachTo();
	Reset_DoOnce_Physics();
}

// Called every frame
void ABasicItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector LocalCurrentPosition = MainMesh->GetComponentLocation();
	Velocity = (LastFramePosition.Size() - LocalCurrentPosition.Size());
	Velocity3D = GetActorTransform().TransformVectorNoScale(FVector(LastFramePosition.Y - LocalCurrentPosition.Y, LastFramePosition.Z - LocalCurrentPosition.Z, LastFramePosition.X - LocalCurrentPosition.X));
	LastFramePosition = LocalCurrentPosition;


	if (bIsGrabbed)
	{
		switch (GrabMethod)
		{
		case EGrabType::Physics:
			if (CheckHandDistance(DropDistance))
			{
				Drop();
			}
			break;
		case EGrabType::Mixed:
			HandleMixedPhysics();
			break;
		default: ;
		}
	}
}