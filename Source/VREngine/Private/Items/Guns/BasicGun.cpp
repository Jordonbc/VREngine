// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Guns/BasicGun.h"
#include "Items/Guns/BasicAmmo.h"
#include "Items/Guns/BasicAttachment.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Player/BaseMotionController.h"
#include "Components/CapsuleComponent.h"
#include "DebugMacros.h"
#include "Interfaces/VRDualHands.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "GameFramework/DamageType.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Items/Guns/BasicBulletShell.h"
#include "Components/TimelineComponent.h"
#include "GlobalFunctionLibrary.h"
//#include "Log.h"
#include "Player/BasePlayerPawn.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Items/Guns/BasicPhysicalBullet.h"

DEFINE_LOG_CATEGORY_STATIC(BasicGunLog, All, All)

ABasicGun::ABasicGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SI_LOG("Creating Box Component");
	GunHandle = CreateDefaultSubobject<UBoxComponent>(TEXT("GunHandle"));
	GunHandle->SetupAttachment(MainMesh);
	GunHandle->ComponentTags.Add(FName("Grip"));
	GunHandle->ComponentTags.Add(FName("Center"));
	GunHandle->SetRelativeRotation(FRotator(-9.999984, 0.0, 0.0));
	GunHandle->SetBoxExtent(FVector(3.0f, 3.0f, 5.0f));
	GunHandle->SetRelativeLocation(FVector(-1.20118, -0.707862, -2.724649), false);

	ShootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ShootSphere"));
	ShootSphere->SetupAttachment(MainMesh);
	ShootSphere->SetRelativeLocation(FVector(16.597584, -0.520334, 3.551846));

	if (HasAmmoCollision)
	{
		AmmoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AmmoCollision"));
		AmmoCollision->SetupAttachment(MainMesh);
		AmmoCollision->SetBoxExtent(FVector(3.0f, 3.0f, 5.0f));
		AmmoCollision->SetRelativeRotation(FRotator(-9.999984, 0.0, 0.0));
		AmmoCollision->SetRelativeLocation(FVector(-1.20118, -0.707862, -2.724649), false);
	}

	ReloadCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadCollisionBox"));
	ReloadCollisionBox->SetupAttachment(MainMesh);
	ReloadCollisionBox->SetBoxExtent(FVector(10.0f, 2.0f, 2.0f));
	ReloadCollisionBox->SetRelativeLocation(FVector(5.496763, -0.707877	, 4.570799), false);

	BulletShellSpawn = CreateDefaultSubobject<USphereComponent>(TEXT("BulletShellSpawn"));
	BulletShellSpawn->SetupAttachment(MainMesh);
	BulletShellSpawn->SetSphereRadius(1.0f);
	BulletShellSpawn->SetRelativeLocation(FVector(6.346931, 2.835759, 4.990511), false);
	BulletShellSpawn->SetRelativeRotation(FRotator(0.000013, 30.000072, 90.000282));

	MaxReload = CreateDefaultSubobject<USphereComponent>(TEXT("MaxReload"));
	MaxReload->SetupAttachment(MainMesh);
	MaxReload->SetSphereRadius(2.0f);
	MaxReload->SetRelativeLocation(FVector(-1.20118, -0.707862, -2.724649), false);

	ReloadEnd = CreateDefaultSubobject<USphereComponent>(TEXT("ReloadEnd"));
	ReloadEnd->SetupAttachment(MainMesh);
	ReloadEnd->SetSphereRadius(2.0f);
	ReloadEnd->SetRelativeLocation(FVector(-1.20118, -0.707862, -2.724649), false);

	static ConstructorHelpers::FObjectFinder<USoundBase>EmptySoundObject(TEXT("SoundWave'/Game/Audio/Guns/DryGun.DryGun'"));
	EmptySound = EmptySoundObject.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>HitSoundObject(TEXT("SoundCue'/Game/Audio/Guns/BulletHit_Cue.BulletHit_Cue'"));
	HitSound = HitSoundObject.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>ShootSoundObject(TEXT("SoundWave'/Game/Audio/Guns/Scar-H.Scar-H'"));
	ShootSound = ShootSoundObject.Object;

	AmmoCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicGun::OnAmmoBoxComponentBeginOverlap);

	ReloadCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABasicGun::OnReloadCollisionBeginOverlap);

	ReloadCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABasicGun::OnReloadCollisionEndOverlap);

	EmptyAnimationCurve = CreateDefaultSubobject<UTimelineComponent>(TEXT("EmptyTimeline"));

	EmptyAnimationCurveProgress.BindUFunction(this, FName("EmptyAnimationCurve_Update"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat>EmptyCurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/Items/CppTesting/EmptyAnimationCurve.EmptyAnimationCurve'"));
	UCurveFloat* EmptyCurveFloat = EmptyCurveFloatObject.Object;
	EmptyAnimationCurve->AddInterpFloat(EmptyCurveFloat, EmptyAnimationCurveProgress, FName("Value"));
	EmptyAnimationCurve->SetLooping(false);


	FullReloadAnimationCurve = CreateDefaultSubobject<UTimelineComponent>(TEXT("FullReloadTimeline"));

	FullReloadAnimationCurveProgress.BindUFunction(this, "FullReloadAnimationCurve_Update");
	static ConstructorHelpers::FObjectFinder<UCurveFloat>FullReloadCurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/Items/CppTesting/FullReloadAnimationCurve.FullReloadAnimationCurve'"));
	UCurveFloat* FullReloadCurveFloat = FullReloadCurveFloatObject.Object;
	FullReloadAnimationCurve->AddInterpFloat(FullReloadCurveFloat, FullReloadAnimationCurveProgress, FName("Value"));
	FullReloadAnimationCurve->SetLooping(false);

	FullReloadAnimation_StartEvent.BindUFunction(this, FName("FullReloadAnimation_Start"));
	FullReloadAnimation_EndEvent.BindUFunction(this, FName("FullReloadAnimation_End"));

	FullReloadAnimationCurve->SetTimelineFinishedFunc(FullReloadAnimation_EndEvent);


	static ConstructorHelpers::FObjectFinder<UParticleSystem>FiringPS_Object(TEXT("ParticleSystem'/Game/Meshes/Effects/V2/P_Fire.P_Fire'"));
	FiringPS = FiringPS_Object.Object;

	PhysicalBulletClass = ABasicPhysicalBullet::StaticClass();

	ClassesToIgnore.Add(ABasicBulletShell::StaticClass());

}

void ABasicGun::OnActivate_Implementation()
{
	Super::OnActivate_Implementation();
	
	switch (FireMode)
	{
	case EFireMode::FullAuto:
		bAutomaticFire = true;
		break;
	default:
		PreShoot_Implementation();
		break;
	}
}

void ABasicGun::OnDeactivate_Implementation()
{
	bAutomaticFire = false;
}

void ABasicGun::SetupParams()
{
	Super::SetupParams();
}

void ABasicGun::Shoot_Implementation()
{
	bFiring = true;
	//MotionControllerBP->RumbleController()

	if (bLineTraced)
	{
		TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> LocalActorsToIgnore;
		LocalActorsToIgnore.Add(this);

		FHitResult Results;

		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), ShootSphere->GetComponentLocation(), (ShootSphere->GetForwardVector() * (Range * 1000)), ObjectTypes, true, LocalActorsToIgnore, EDrawDebugTrace::ForOneFrame, Results, true))
		{
			if (Results.Component->IsSimulatingPhysics())
			{
				Results.Component->AddImpulseAtLocation(((Results.ImpactNormal * -1) * 10) * ForceAppliedToObjects, Results.ImpactPoint);
			}

			TSubclassOf<UDamageType> ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			//Results.GetActor()->TakeDamage(Damage, DamageEvent, GetWorld()->GetFirstPlayerController(), (AActor*)MotionControllerBP->Player);

			if (IsValid(Results.GetActor()))
			{
				if (IDamage* Interface = Cast<IDamage>(Results.GetActor()))
				{
					//Interface->RecieveDamage(Damage * DamageMultiplier, Results);
					Interface->Execute_RecieveDamage(Results.GetActor(), Damage * DamageMultiplier, Results);
				}
				else
				{
					if (IsValid(MotionControllerBP))
					{
						Results.GetActor()->TakeDamage(Damage * DamageMultiplier, DamageEvent, GetWorld()->GetFirstPlayerController(), (AActor*)MotionControllerBP->GetInstigator());
					}
				}
			}

			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Results.ImpactPoint);
		}
	}
	else if (IsValid(PhysicalBulletClass))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bDeferConstruction = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABasicPhysicalBullet* Bullet = GetWorld()->SpawnActor<ABasicPhysicalBullet>(PhysicalBulletClass->GetDefaultObject()->GetClass(), SpawnParams);

		Bullet->FinishSpawning(FTransform(ShootSphere->GetComponentRotation(), ShootSphere->GetComponentLocation(), FVector(ShellSize)));
	}
	else  UE_LOG(LogTemp, VeryVerbose, TEXT("'PhysicalBulletClass' appears to be invalid class!"));  //ULog::Error("'PhysicalBulletClass' appears to be invalid class!");

	bFiring = false;

	UGameplayStatics::SpawnSoundAttached(ShootSound, MainMesh);
	if (FMath::RandRange(0, FireEffectProbability) == 0)
	{
		UGameplayStatics::SpawnEmitterAttached(FiringPS, ShootSphere, FName(""), FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f), EAttachLocation::SnapToTarget, true, EPSCPoolMethod::AutoRelease, true);
	}

	//ameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSound, ShootSphere->GetComponentLocation());

	if (bSpawnShells) EjectShell();
	else InternalAmmo--;

	if (!(FireMode == EFireMode::SingleShot))
	{
		FillInternalAmmo();
	}
}

void ABasicGun::PreShoot_Implementation()
{
	if (bCanFire)
	{
		switch (FireMode)
		{
		case EFireMode::DoubleAction:
			if (InternalAmmo <= 0)
			{
				bool a = FillInternalAmmo();
				if (a)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptySound, ShootSphere->GetComponentLocation());
					EmptyAnimationCurve->ReverseFromEnd();
					return;
				}
				else OnEmpty();
				return;
			}
			break;

		case EFireMode::SingleShot:
			if (InternalAmmo <= 0)
			{
				OnEmpty();
				return;
			}
			break;

		case EFireMode::FullAuto:
			if (InternalAmmo <= 0)
			{
				OnEmpty();
				return;
			}
			break;
		}

		if (InternalAmmo > 0)
		{
			ShootAnimation(false);
		}
		else
		{
			OnEmpty();
		}
	}
}

float ABasicGun::GetRPM()
{
	return ((60 / RoundsPerMinute));
}

bool ABasicGun::PreInitAttachment(ABasicAttachment* Attachment)
{
	if (IsValid(Attachment))
	{
		if (Attachment->bIsGrabbed)
		{
			Attachment->Drop();
		}
		Attachment->Gun = this;

		return true;
	}
	return false;
}

bool ABasicGun::AttachAttachment(ABasicAttachment* Attachment, FName SocketName)
{
	bool Attached;

	if (IsValid(Attachment))
	{
		UPrimitiveComponent* ComponentToAttach = (UPrimitiveComponent*)Attachment->GetRootComponent();

		ComponentToAttach->SetCollisionObjectType(ECC_GameTraceChannel2);
		ComponentToAttach->SetEnableGravity(false);
		ComponentToAttach->SetSimulatePhysics(false);
		FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		switch (Attachment->Attachment.Position)
		{
		case EAttachmentPosition::Sight:
			Attached = ComponentToAttach->AttachToComponent(MainMesh, Rules, FName("TopRailSocket"));
			break;
		case EAttachmentPosition::Ammo:
			Attached = ComponentToAttach->AttachToComponent(MainMesh, Rules, FName("AmmoSocket"));
			break;
		case EAttachmentPosition::Front:
			Attached = ComponentToAttach->AttachToComponent(MainMesh, Rules, FName("FrontRailSocket"));
			break;
		case EAttachmentPosition::Bottom:
			Attached = ComponentToAttach->AttachToComponent(MainMesh, Rules, FName("BottomRailSocket"));
			break;
		default:
			Attached = ComponentToAttach->AttachToComponent(MainMesh, Rules, SocketName);
			break;
		}
		
		Attachments.Add(Attachment->Attachment);

		return true;
	}
	UE_LOG(LogTemp, VeryVerbose, TEXT("WARN: Unabable to attach component to gun!"));
	return false;
	//ULog::Warning(FString("WARN: Unabable to attach component to gun!"));
}

bool ABasicGun::DetachAttachment(FAttachment AttachmentToDetach)
{
	if (IsValid(AttachmentToDetach.AttachmentActor))
	{

		for (int i = 0; i < Attachments.Num(); i++)
		{
			if (Attachments[i].AttachmentActor == AttachmentToDetach.AttachmentActor)
			{
				AttachmentToDetach.AttachmentActor->Gun = nullptr;
				AttachmentToDetach.AttachmentActor->bAttachedToGun = false;
				AttachmentToDetach.AttachmentActor->DropAttachTo();

				Attachments.RemoveAt(i);
				return true;
			}
		}

		UE_LOG(LogTemp, VeryVerbose, TEXT("FATAL: you are trying to remove an attachment that is not attached to the gun!"));
		return false;
		//ULog::Fatal(FString("FATAL: you are trying to remove an attachment that is not attached to the gun!"));
	}
	else return false;
}

bool ABasicGun::DetachItem(ABasicAttachment* Attachment)
{
	if (IsValid(Attachment))
	{
		if (ABasicAmmo* a = Cast<ABasicAmmo>(Attachment)) DetachAttachment(a);
		//else if (Attachment->GetClass() == TSubclassOf<ABasicAmmo>()) DetachAmmo();
		return true;
	}
	return false;
}

void ABasicGun::OnAmmoAttached_Implementation()
{
}

void ABasicGun::OnAmmoRemoved_Implementation()
{
}

void ABasicGun::EmptyAnimationCurve_Update(float value)
{
	if (value > GetMaxReloadDistance())
	{
		ReloadValue = value;
	}
	else
	{
		ReloadValue = GetMaxReloadDistance();
	}
}

void ABasicGun::FullReloadAnimationCurve_Update(float value)
{
	ReloadValue = value * GetMaxReloadDistance();
}

void ABasicGun::FullReloadAnimation_Start()
{
	Shoot();
}

void ABasicGun::FullReloadAnimation_End()
{
	bCanFire = true;
}

bool ABasicGun::FillInternalAmmo()
{
	FAttachment AmmoAttachment = GetAmmoAttachment();
	if (ABasicAmmo* AmmoActor = Cast<ABasicAmmo>(AmmoAttachment.AttachmentActor))
	{
		if (AmmoActor->CurrentAmmo > 0)
		{
			if (InternalAmmo < MaxInternalAmmo)
			{
				uint8 AmmoWanted = MaxInternalAmmo - InternalAmmo;

				if (AmmoActor->CurrentAmmo >= AmmoWanted)
				{
					AmmoActor->CurrentAmmo -= AmmoWanted;
					InternalAmmo += AmmoWanted;
				}
				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}

void ABasicGun::ShootTick()
{
	if (FireMode == EFireMode::FullAuto)
	{
		if (bAutomaticFire)
		{
			if (Timer == 0.0f || Timer + GetRPM() <= UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()))
			{
				Timer = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
				PreShoot_Implementation();
			}
		}
	}
}

void ABasicGun::OnAmmoBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (IsValid(OtherActor))
	{
		if (ABasicAmmo* AsAmmo = Cast<ABasicAmmo>(OtherActor))
		{
			if (bConsumeAmmo)
			{
				//if (CompatibleAmmoTypes.ContainsByPredicate([AsAmmo](TSubclassOf<ABasicAmmo> Element) { return Element.Get() == AsAmmo->GetClass(); }))
				if(UGlobalFunctionLibrary::ArrayContainsClass<ABasicAmmo>(CompatibleAmmoTypes, AsAmmo->GetClass()))
				{
					FillInternalAmmo();
				}
			}
			else
			{
				if (!AsAmmo->bAttachedToGun)
				{
					FAttachment AmmoAttachment = GetAmmoAttachment();

					if (AmmoAttachment.AttachmentActor != nullptr)
					{
						//UE_LOG(LogTemp, VeryVerbose, TEXT("");
						//ULog::DebugMessage(EDebugLogType::DL_Info, AmmoAttachment.AttachmentActor->GetName());

						if (ABasicAmmo* AmmoActor = Cast<ABasicAmmo>(AmmoAttachment.AttachmentActor))
						{
							if (IsValid(AmmoActor))
							{
								DetachAttachment(AmmoActor);
								//RemoveAmmo(AmmoActor);
							}
						}
					}

					

					//if (CompatibleAmmoTypes.Contains(AsAmmo->GetClass())) // PROBLEM CODE!!
					//if (CompatibleAmmoTypes.ContainsByPredicate([AsAmmo](TSubclassOf<ABasicAmmo> Element) { return Element.Get() == AsAmmo->GetClass(); }))
					if(UGlobalFunctionLibrary::ArrayContainsClass<ABasicAmmo>(CompatibleAmmoTypes, AsAmmo->GetClass()))
					{
						PreInitAttachment(AsAmmo);
							if (AttachAttachment(AsAmmo, FName("AmmoSocket")))
							{
								AsAmmo->bAttachedToGun = true;
								//FillInternalAmmo(); Dont refill internal ammo after loaded a magazine
								OnAmmoAttached_Implementation();
							}
							else DetachAttachment(AsAmmo);
					}
				}
			}
		}
	}
}

void ABasicGun::OnReloadCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (!IsValid(ReloadMotionController))
		{
			if (IsValid(MotionControllerBP))
			{
				if (OtherActor != MotionControllerBP)
				{
					if (!bFiring)
					{
						ReloadMotionController = Cast<ABaseMotionController>(OtherActor);
					}
				}
			}

		}
	}
}

void ABasicGun::OnReloadCollisionEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor))
	{
		if (Cast<ABaseMotionController>(OtherActor))
		{
			if (IsValid(ReloadMotionController))
			{
				if (!bFiring)
				{
					if (ReloadValueNormalized <= 0.5f) ReloadValue = 0.0f;
					else ReloadValue = GetMaxReloadDistance();
					FullReloadAnimationCurve->SetPlaybackPosition(UKismetMathLibrary::MapRangeClamped(ReloadValue, 0.0f, GetMaxReloadDistance(), 0.0f, 1.0f), true, true);
					//FullReloadAnimationCurve->Play();
				}
				ReloadMotionController = nullptr;
			}
		}
	}
}

float ABasicGun::GetDistanceOfHand()
{

	return UKismetMathLibrary::InverseTransformLocation(ReloadEnd->GetComponentTransform(), ReloadMotionController->MotionControllerComponent->GetComponentLocation()).X;
}

float ABasicGun::GetMaxReloadDistance()
{
	return UKismetMathLibrary::InverseTransformLocation(ReloadEnd->GetComponentTransform(), MaxReload->GetComponentLocation()).X;
}

bool ABasicGun::PullbackDistance()
{

	return ReloadValue <= GetMaxReloadDistance() + (GetMaxReloadDistance() / 3.0f);
}

void ABasicGun::DOONCE_HASRELOADED()
{
	if (!DOONCE_HASRELOADED_BOOL)
	{
		//bReloaded = true;
		FillInternalAmmo();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound1, GetActorLocation());

		DOONCE_HASRELOADED_BOOL = true;
	}
}

void ABasicGun::DOONCE_HASRELOADED_RESET()
{
	DOONCE_HASRELOADED_BOOL = false;
}

void ABasicGun::DOONCE_ISRELOADING()
{

	if (!DOONCE_ISRELOADING_BOOL)
	{
		EjectShell_Implementation();

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound1, GetActorLocation());

		DOONCE_ISRELOADING_BOOL = true;

		DOONCE_HASRELOADED_RESET();
	}
}

void ABasicGun::DOONCE_ISRELOADING_RESET()
{
	DOONCE_ISRELOADING_BOOL = false;
}

void ABasicGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*EmptyAnimationCurveProgress->TickTimeline(DeltaTime);
	FullReloadAnimationCurveProgress->TickTimeline(DeltaTime);*/

	if (bIsGrabbed)
	{

		ReloadValueNormalized = UKismetMathLibrary::MapRangeClamped(ReloadValue, 0, GetMaxReloadDistance(), 0, 1);

		// Snaps the value if close to min or max
		if (ReloadValueNormalized >= 0.9f && ReloadValueNormalized < 1.0f) ReloadValueNormalized = 1.0f;
		else if (ReloadValueNormalized <= 0.1f && ReloadValueNormalized > 0.0f) ReloadValueNormalized = 0.0f;

		//TODO: Don't be lazy here and get a dedicated GetPlayer global function!
		TriggerValue = MotionControllerBP->Player->GetTriggerValue(MotionControllerBP);

		if (ReloadMotionController && !bFiring)
		{
			if (InternalAmmo <= 0 && ReloadValueNormalized <= 0.25)
			{
				bool a = FillInternalAmmo();
				if (a) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound1, GetActorLocation());
			}
			ReloadValue = GetDistanceOfHand();

			//Clamp ReloadValue
			ReloadValue = UKismetMathLibrary::Clamp(GetDistanceOfHand(), GetMaxReloadDistance(), 0.0f);
		}

		ShootTick();


		if (bFiring)
		{

			if (!IsValid(SecondHandMotionController))
			{
				if (UKismetMathLibrary::EqualEqual_RotatorRotator(MainMesh->GetComponentRotation(), FRotator()))
				{
					FRotator InterpToRotation = UKismetMathLibrary::RInterpTo_Constant(MainMesh->GetComponentRotation(), FRotator(), GetWorld()->GetDeltaSeconds(), CooldownSpeed);

					MainMesh->SetWorldRotation(InterpToRotation, true, (FHitResult*)nullptr, ETeleportType::None);
				}
			}


			

			if (IsValid(SecondHandMotionController)) MainMesh->SetWorldRotation(FQuat(FRotator(SecondHandMotionController->MotionControllerComponent->GetComponentRotation().Pitch, SecondHandMotionController->MotionControllerComponent->GetComponentRotation().Yaw, MotionControllerBP->MotionControllerComponent->GetComponentRotation().Roll)));

			if (bHasReloadingPart)
			{
				if (IsValid(ReloadMotionController))
				{
					

			//		if (ReloadValue >= GetMaxReloadDistance() / 3.0f)
			//		{
			//			DOONCE_HASRELOADED();
			//			
			//		}
			//		else
			//		{
			//			if (PullbackDistance())
			//			{
			//				DOONCE_ISRELOADING();
			//			}
			//			else
			//			{
			//				DOONCE_HASRELOADED_RESET();
			//				DOONCE_ISRELOADING_RESET();
			//			}
			//		}
			//	}
			//	else
			//	{
			//		//if (!bFiring) ReloadValue = bReloaded ? 0.0f : GetMaxReloadDistance();
				}
			}
		}
		else
		{
			if (IVRDualHands* Interface = Cast<IVRDualHands>(this))
			{
				if (Interface->Execute_HasSecondHand(this))
				{
					//TODO: Second hand pickup logic here
				}
			}
			else
			{
				ShootTick();
			}
		}
	}
}

void ABasicGun::BeginPlay()
{
	Super::BeginPlay();
	ReloadValue = UKismetMathLibrary::InverseTransformLocation(ReloadEnd->GetComponentTransform(), MaxReload->GetComponentLocation()).X;
}

void ABasicGun::SecondHand_Pickup_Implementation(USceneComponent* MotionController, ABaseMotionController* MotionControllerActor)
{
	if (IsValid(MotionControllerActor))
	{
		SecondHandMotionController = MotionControllerActor;
	}
}

void ABasicGun::SecondHand_Drop_Implementation()
{
	SecondHandMotionController = nullptr;
}

bool ABasicGun::SecondHand_IsHeldByMe_Implementation(UMotionControllerComponent* MotionController)
{
	if (IsValid(MotionController) && IsValid(SecondHandMotionController))
	{
		if (SecondHandMotionController->MotionControllerComponent == MotionController)
		{
			return true;
		}
	}
	return false;
}

void ABasicGun::EjectShell_Implementation()
{
	if (InternalAmmo > 0)
	{
		InternalAmmo--;

		if (bSpawnShells)
		{
			if (IsValid(EmptyBulletShellClass))
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.bDeferConstruction = true;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				ABasicBulletShell* BulletShell = GetWorld()->SpawnActor<ABasicBulletShell>(EmptyBulletShellClass->GetDefaultObject()->GetClass(), SpawnParams);
				//BulletShell->Forward = BulletShellSpawn->GetForwardVector();
				BulletShell->FinishSpawning(FTransform(BulletShellSpawn->GetComponentRotation(), BulletShellSpawn->GetComponentLocation(), FVector(ShellSize)));
			}
		}
	}
	else if (FillInternalAmmo()) EjectShell_Implementation();
}

void ABasicGun::OnEmpty_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptySound, ShootSphere->GetComponentLocation());
	ShootAnimation(true);
}

void ABasicGun::ShootAnimation_Implementation(bool IsEmpty)
{
	if (IsEmpty)
	{
		if ((ReloadValue >= 0.0f)) EmptyAnimationCurve->PlayFromStart();

	}
	else
	{
		FullReloadAnimationCurve->SetPlayRate(RoundsPerMinute / 60);
		if (!(FireMode == EFireMode::FullAuto) || !(FireMode == EFireMode::DoubleAction))
		{
			if (bCanFire)
			{
				bCanFire = false;
			}
			FullReloadAnimationCurve->PlayFromStart();
			FullReloadAnimation_Start();
		}
	}
}

FAttachment ABasicGun::GetAmmoAttachment()
{
	for (int i = 0; i < Attachments.Num(); i++)
	{
		if (ABasicAmmo* ammo = Cast<ABasicAmmo>(Attachments[i].AttachmentActor)) return ammo;
	}

	//if (Attachments.ContainsByPredicate([](TSubclassOf<ABasicAmmo> Element) { return Element.Get() == AsAmmo->GetClass(); }))

	//ABasicAmmo* AsAmmo = Cast<ABasicAmmo>(OtherActor)

	return FAttachment(nullptr);
}
