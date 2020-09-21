// Copyright 2017-2020 Jordon Brooks.


#include "Items/Guns/BasicBulletShell.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABasicBulletShell::ABasicBulletShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultStaticMeshObject(TEXT("StaticMesh'/Game/Meshes/Weapons/Ranged/Bullets/51mm/51mm_-_shell.51mm_-_shell'"));
	GetStaticMeshComponent()->SetStaticMesh(DefaultStaticMeshObject.Object);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	ProjectileMovementComponent->InitialSpeed = 200.0f;
	ProjectileMovementComponent->MaxSpeed = 100.0f;
	ProjectileMovementComponent->Bounciness = 0.8f;
	ProjectileMovementComponent->Velocity = FVector(0.0f, 1.0f, 0.0f);

	MainMesh->BodyInstance.bOverrideMass = true;
	MainMesh->BodyInstance.SetMassOverride(0.1f);

	static ConstructorHelpers::FObjectFinder<USoundBase>HitSoundObject(TEXT("SoundCue'/Game/Audio/Guns/BulletShell_Cue.BulletShell_Cue'"));
	HitSound = HitSoundObject.Object;

	MainMesh->OnComponentHit.AddDynamic(this, &ABasicBulletShell::OnBulletComponentHit);

}

// Called when the game starts or when spawned
void ABasicBulletShell::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerDeactivateProjectileMovementComponent, this, &ABasicBulletShell::DeactivateProjectileMovementComponent, 0.01f, false);
}

void ABasicBulletShell::OnBulletComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HitTime1 == 0.0f)
	{
		HitTime1 = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitSound, Hit.Location);
	}
}

// Called every frame
void ABasicBulletShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicBulletShell::DeactivateProjectileMovementComponent()
{
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->Deactivate();
}

