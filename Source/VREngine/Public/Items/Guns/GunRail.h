// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GunRail.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRENGINE_API UGunRail : public UBoxComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGunRail();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bRequireGunHeld = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TArray<TSubclassOf<class ABasicAttachment>> CompatibleAttachmentClasses;

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Collision")
		class ABasicGun* Gun;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
		void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
