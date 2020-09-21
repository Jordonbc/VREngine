// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/StaticMeshItem.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "BasicAttachment.generated.h"

/**
 * 
 */
UCLASS(abstract)
class VRENGINE_API ABasicAttachment : public AStaticMeshItem
{
	GENERATED_BODY()
public:
		ABasicAttachment();

		UPROPERTY(BlueprintReadOnly, Category = "Attachment Stats")
			class ABasicGun* Gun;

		UPROPERTY(BlueprintReadOnly, Category = "Attachment Stats")
			bool bAttachedToGun = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment Stats")
			FAttachment Attachment = FAttachment(this, EAttachmentPosition::Other);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment Stats")
			bool bRequireGunHeld = true;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment Stats")
			bool bIsOn = false;

		UFUNCTION(BlueprintImplementableEvent, Category = "Attachment Functions")
			void TurnOn();

		UFUNCTION(BlueprintImplementableEvent, Category = "Attachment Functions")
			void TurnOff();

		virtual void OnActivate_Implementation() override;

		virtual void OnDeactivate_Implementation() override;

		virtual void Pickup_Implementation(class ABaseMotionController* MotionControllerActor, bool Center) override;

		virtual void Drop_Implementation() override;

		virtual void HoverTrigger_Implementation(ABaseMotionController* MotionControllerActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
