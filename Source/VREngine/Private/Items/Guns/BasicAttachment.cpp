// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Guns/BasicAttachment.h"
#include "Items/Guns/BasicGun.h"

ABasicAttachment::ABasicAttachment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (bIsOn) TurnOn();
	else TurnOff();
}

void ABasicAttachment::OnActivate_Implementation()
{
	Super::OnActivate_Implementation();

	if (bIsOn)
	{
		TurnOff();
		bIsOn = false;
	}
	else
	{
		TurnOn();
		bIsOn = true;
	}
}

void ABasicAttachment::OnDeactivate_Implementation()
{
	Super::OnDeactivate_Implementation();
}

void ABasicAttachment::Pickup_Implementation(ABaseMotionController* MotionControllerActor, bool Center)
{
	if (IsValid(Gun))
	{
		if (bRequireGunHeld && Gun->bIsGrabbed || !bRequireGunHeld && Gun->bIsGrabbed || !bRequireGunHeld && !Gun->bIsGrabbed) // Only detach ammo from gun if the gun is being held
		{
			//Gun->DetachItem(this);
			//bAttachedToGun = false;
			//Gun = nullptr;
			Gun->DetachAttachment(Attachment);

			Super::Pickup_Implementation(MotionControllerActor, Center);
		}
	}
	else
	{
		Super::Pickup_Implementation(MotionControllerActor, Center);
	}
}

void ABasicAttachment::Drop_Implementation()
{
	Super::Drop_Implementation();
}

void ABasicAttachment::HoverTrigger_Implementation(ABaseMotionController* MotionControllerActor)
{
	Super::HoverTrigger_Implementation(MotionControllerActor);
	if (bIsOn) TurnOff();
	else TurnOn();
}

void ABasicAttachment::BeginPlay()
{
	Super::BeginPlay();
}

void ABasicAttachment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
