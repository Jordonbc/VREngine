// Copyright 2017-2020 Jordon Brooks.


#include "Items/Guns/BasicPhysicalBullet.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Items/Guns/BasicGun.h"
#include "Interfaces/Damage.h"
#include "GameFramework/DamageType.h"
#include "GlobalFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ABasicPhysicalBullet::ABasicPhysicalBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	ProjectileMovementComponent->InitialSpeed = 8000.0f;
	ProjectileMovementComponent->MaxSpeed = 8000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0025f;
	ProjectileMovementComponent->Bounciness = 0.0f;
	ProjectileMovementComponent->Friction = 0.0f;
	ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 0.0f;
	ProjectileMovementComponent->bForceSubStepping = true;
	ProjectileMovementComponent->BounceAdditionalIterations = 0.0f;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;
	ProjectileMovementComponent->Velocity = FVector(1.0f, 0.0f, 0.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultStaticMeshObject(TEXT("StaticMesh'/Game/Meshes/Weapons/Ranged/Bullets/51mm/51mm_-_bullet.51mm_-_bullet'"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	StaticMeshComponent->SetStaticMesh(DefaultStaticMeshObject.Object);

	StaticMeshComponent->SetCollisionProfileName(FName("PhysicsActor"));
	StaticMeshComponent->CanCharacterStepUpOn = ECB_No;
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);

	static ConstructorHelpers::FObjectFinder<USoundBase>HitSoundObject(TEXT("SoundCue'/Game/Audio/Guns/BulletHit_Cue.BulletHit_Cue'"));
	HitSound = HitSoundObject.Object;

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ABasicPhysicalBullet::OnComponentHit);

}

// Called when the game starts or when spawned
void ABasicPhysicalBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicPhysicalBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicPhysicalBullet::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(GunReference) && IsValid(OtherActor))
	{
		if (!(OtherActor == GunReference))
		{
			if (HitComp->IsSimulatingPhysics())
			{
				HitComp->AddImpulseAtLocation(((Hit.ImpactNormal * -1) * 10) * GunReference->ForceAppliedToObjects, Hit.ImpactPoint);
			}

			TSubclassOf<UDamageType> ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);

			if (IDamage* Interface = Cast<IDamage>(Hit.GetActor()))
			{
				//Interface->RecieveDamage(Damage * DamageMultiplier, Results);
				Interface->Execute_RecieveDamage(Hit.GetActor(), GunReference->Damage * GunReference->DamageMultiplier, Hit);
			}
			else
			{
				if (IsValid(GunReference->MotionControllerBP))
				{
					OtherActor->TakeDamage(GunReference->Damage * GunReference->DamageMultiplier, DamageEvent, (AController*)UGlobalFunctionLibrary::GetPlayerController(GetWorld()) , (AActor*)GunReference->MotionControllerBP->GetInstigator());
				}
			}

			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitSound, Hit.Location);
			Destroy();
		}
	}
}

