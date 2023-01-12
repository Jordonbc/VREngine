// Copyright 2017-2022 Jordon Brooks.

#include "Player/BaseMotionController.h"

#include "Components/SphereComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/VRDualHands.h"

DEFINE_LOG_CATEGORY_STATIC(MotionControllerGrabbing, All, All)

void ABaseMotionController::GrabActor()
{
	bWantsToGrip = true;
	const FNearestActorRet NearestActor = GetActorNearHand();
	if ((IsValid(NearestActor.NearestActor)))
	{
		UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Grabbing %s"), *NearestActor.NearestActor->GetName());
		GrabSpecificActor(NearestActor.NearestActor, NearestActor.InteractedComponent);
	}
	else
	{
		UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Grabbing AIR"));
	}
}

void ABaseMotionController::ReleaseActor()
{
	bWantsToGrip = false;
	if (IsValid(AttachedItem))
	{
		if (AttachedItem->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			UE_LOG(MotionControllerGrabbing , Verbose, TEXT("Releasing %s"), *AttachedItem->GetName());

			if (const IVRDualHands* VRDualHandsInterface = Cast<IVRDualHands>(AttachedItem))
			{
				if (!IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
				{
					VRDualHandsInterface->Execute_SecondHand_Drop(AttachedItem);
					bHasGrabbedObject = false;
				}
				else
				{
					if (IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
					{
						IInteractionInterface::Execute_Drop(AttachedItem);
						UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Releasing Actor: %s"), *AttachedItem->GetHumanReadableName());
					}
				}
			}
			else
			{
				if (IInteractionInterface::Execute_IsHeldByMe(AttachedItem, this))
				{
					IInteractionInterface::Execute_Drop(AttachedItem);
					UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Releasing Actor: %s"), *AttachedItem->GetHumanReadableName());
				}
			}

			bHasGrabbedObject = false;
			RumbleController(RumbleHapticEffect, 0.2f, false);

			AttachedItem = nullptr;
			bIsSecondHand = false;
		}
	}
}

FNearestActorRet ABaseMotionController::GetActorNearHand()
{
	FNearestActorRet NearestActor = FNearestActorRet();

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		// Filter out objects not contained in the target world.
		if (Component->GetWorld() != GetWorld())
		{
			continue;
		}
		// Do stuff
		AActor* Actor = Component->GetOwner();

		if (Component->ComponentHasTag(FName("SecondHand")))
		{
			if (Actor->GetClass()->ImplementsInterface(UVRDualHands::StaticClass()))
			{
				const float Vect = FVector::Distance(Actor->GetActorLocation(), GrabSphereComponent->GetComponentLocation());
				if (Vect < NearestActor.DistanceToActor)
				{
					NearestActor.NearestActor = Actor;
					NearestActor.InteractedComponent = Component;
					NearestActor.DistanceToActor = Vect;
				}
			}
		}
		else
		{
			if (Component->ComponentHasTag(FName("Grip")))
			{
				const float Vect = FVector::Distance(Actor->GetActorLocation(), GrabSphereComponent->GetComponentLocation());
				if (Vect < NearestActor.DistanceToActor)
				{
					NearestActor.NearestActor = Actor;
					NearestActor.InteractedComponent = Component;
					NearestActor.DistanceToActor = Vect;
				}
			}
		}
	}

	return NearestActor;
}

void ABaseMotionController::GrabsphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != (AActor*)Player || OtherActor != this)
	{
		OverlappingComponents.Add(OtherComp);
	}
}

void ABaseMotionController::GrabsphereComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingComponents.Remove(OtherComp);
}

void ABaseMotionController::DoOnce_HasGrabbedActorB_Func()
{
	if (!DoOnce_HasGrabbedActorB)
	{
		if (IsValid(HoveringItem))
		{
			if (const IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HoveringItem))
			{
				InteractionInterface->Execute_HoverEnd(HoveringItem, this);
				UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Hover End: %s"), *HoveringItem->GetHumanReadableName());
				HoveringItem = nullptr;
			}
		}
		DoOnce_HasGrabbedActorB = true;
	}
}

void ABaseMotionController::DoOnce_HasGrabbedActorA_FuncReset()
{
	DoOnce_HasGrabbedActorA = false;
}

void ABaseMotionController::DoOnce_HasGrabbedActorB_FuncReset()
{
	DoOnce_HasGrabbedActorB = false;
}

bool ABaseMotionController::GrabSpecificActor(AActor* ActorToGrab, USceneComponent* InteractedComponent)
{
	if (IsValid(ActorToGrab))
	{
		if (ActorToGrab->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			if (ActorToGrab->GetClass()->ImplementsInterface(UVRDualHands::StaticClass()))
			{
				if (IVRDualHands::Execute_HasFirstHand(ActorToGrab))
				{
					IVRDualHands::Execute_SecondHand_Pickup(ActorToGrab, AttachComponent, this);

					bIsSecondHand = true;
					UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Second hand grabbing Specific Actor: %s"), *ActorToGrab->GetName());

				}
				else
				{
					bIsSecondHand = false;
					IInteractionInterface::Execute_Pickup(ActorToGrab, this, (bool)InteractedComponent->ComponentHasTag(FName("Center")));

					UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Grabbing Specific Actor: %s"), *ActorToGrab->GetName());
				}
			}
			else
			{
				bIsSecondHand = false;
				IInteractionInterface::Execute_Pickup(ActorToGrab, this, (bool)InteractedComponent->ComponentHasTag(FName("Center")));

				UE_LOG(MotionControllerGrabbing, Verbose, TEXT("Grabbing Specific Actor: %s"), *ActorToGrab->GetName());

			}

			RumbleController(RumbleHapticEffect, 0.5f, false);

			AttachedItem = ActorToGrab;
			bHasGrabbedObject = true;

			return true;
		}
	}

	return false;
}