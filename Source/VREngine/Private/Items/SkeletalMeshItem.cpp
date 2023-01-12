// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/SkeletalMeshItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASkeletalMeshItem::ASkeletalMeshItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>DefaultSkeletalMeshObject(TEXT("SkeletalMesh'/VREngine/Meshes/FN_Five-Seven/Pistol.Pistol'"));

	USkeletalMeshComponent* RawSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainMesh")); //Creates a skeletal mesh component
	MainMesh = static_cast<UPrimitiveComponent*>(RawSkeletalMeshComponent);
	
	RawSkeletalMeshComponent->SetSkeletalMesh(DefaultSkeletalMeshObject.Object);

	ABasicItem::SetupParams();
}

// Called when the game starts or when spawned
void ASkeletalMeshItem::BeginPlay()
{
	Super::BeginPlay();
}

void ASkeletalMeshItem::PostInitProperties()
{
	Super::PostInitProperties();

	//for (FName Tag : MainMesh->ComponentTags)
	//{
	//	SI_LOG(FString(Tag.ToString()));
	//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Tag.ToString());
	//}
}

// Called every frame
void ASkeletalMeshItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}