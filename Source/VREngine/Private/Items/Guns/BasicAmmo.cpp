// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Guns/BasicAmmo.h"

ABasicAmmo::ABasicAmmo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CurrentAmmo = MaxAmmoCapacity;
	Attachment.Position = EAttachmentPosition::Ammo;
}