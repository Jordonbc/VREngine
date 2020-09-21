// Copyright 2017-2020 Jordon Brooks.


#include "Items/Guns/GunRail.h"
#include "Items/Guns/BasicGun.h"
#include "Items/Guns/BasicAttachment.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GlobalFunctionLibrary.h"
//#include "Log.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UGunRail::UGunRail()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	OnComponentBeginOverlap.AddDynamic(this, &UGunRail::OnCollisionBeginOverlap);

	BoxExtent = FVector(1.0f, 1.0f, 1.0f);

}


// Called when the game starts
void UGunRail::BeginPlay()
{
	Super::BeginPlay();

	Gun = Cast<ABasicGun>(GetOwner());

	//if (!IsValid(Gun))
	//{
	//	ULog::Error("Parent object is not type 'ABasicGun'!");
	//}
}


// Called every frame
void UGunRail::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGunRail::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (ABasicAttachment* NewAttachment = Cast<ABasicAttachment>(OtherActor))
		{
			if (!NewAttachment->bAttachedToGun)
			{
				//if (CompatibleAttachmentClasses.ContainsByPredicate([NewAttachment](TSubclassOf<ABasicAttachment> Element) { return NewAttachment->GetClass()->IsChildOf(Element); }))
				if(UGlobalFunctionLibrary::ArrayContainsClass<ABasicAttachment>(CompatibleAttachmentClasses, NewAttachment->GetClass()))
				{
					//ULog::Info("True");
					if (NewAttachment->bIsGrabbed) NewAttachment->Drop();
					NewAttachment->Gun = Gun;

					Gun->AttachAttachment(NewAttachment, SocketName);
				}
				//else ULog::Error("False");
			}
		}
	}
}
