// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/StaticMeshItem.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
// "DebugMacros.h"
#include "Engine/StaticMesh.h"

// Sets default values
AStaticMeshItem::AStaticMeshItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultStaticMeshObject(TEXT("StaticMesh'/Game/Geometry/Meshes/1mCube.1mCube'"));

	RawStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh")); //Creates a static mesh component
	MainMesh = (UPrimitiveComponent*)RawStaticMeshComponent;

	RawStaticMeshComponent->SetStaticMesh(DefaultStaticMeshObject.Object);

	SetupParams();
}

UStaticMeshComponent* AStaticMeshItem::GetStaticMeshComponent()
{
	return RawStaticMeshComponent;
}