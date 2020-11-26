// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BasicSword.h"
#include "UObject/ConstructorHelpers.h"
//#include "DebugMacros.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"


ABasicSword::ABasicSword()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultStaticMeshObject(TEXT("StaticMesh'/Game/Meshes/Weapons/Melee/SAO/SAO_Lasersword_Cylinder.SAO_Lasersword_Cylinder'"));
	UStaticMesh* DefaultStaticMesh = DefaultStaticMeshObject.Object;

	GetStaticMeshComponent()->SetStaticMesh(DefaultStaticMesh);

	Handle = CreateDefaultSubobject<UBoxComponent>(TEXT("GunHandle"));
	Handle->SetupAttachment(MainMesh);
	Handle->ComponentTags.Add(FName("Grip"));
	Handle->ComponentTags.Add(FName("Center"));
	Handle->SetRelativeRotation(FRotator(-9.999984, 0.0, 0.0));
	Handle->SetBoxExtent(FVector(3.0f, 3.0f, 5.0f));
	Handle->SetRelativeLocation(FVector(-1.20118, -0.707862, -2.724649), false);
}

void ABasicSword::OnSwordHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}
