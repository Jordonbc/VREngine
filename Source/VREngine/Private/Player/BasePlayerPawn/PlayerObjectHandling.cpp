// Copyright 2017-2022 Jordon Brooks.

#include "Player/BasePlayerPawn.h"
#include "GlobalFunctionLibrary.h"
#include "Interfaces/InteractionInterface.h"
#include "Player/BaseMotionController.h"
#include "Player/VRPlayerController.h"


void ABasePlayerPawn::ActivateLeft_Pressed()
{
	ActivateObject(LeftController);
}

void ABasePlayerPawn::ActivateRight_Pressed()
{
	ActivateObject(RightController);
}

void ABasePlayerPawn::ActivateRight_Released()
{
	DeactivateObject(RightController);
}

void ABasePlayerPawn::ActivateLeft_Released()
{
	DeactivateObject(LeftController);
}

void ABasePlayerPawn::GrabLeft_Released()
{
	ReleaseObject(LeftController);
}

void ABasePlayerPawn::GrabRight_Released()
{
	ReleaseObject(RightController);
}

void ABasePlayerPawn::GrabLeft_Pressed()
{
	GrabObject(LeftController);
}

void ABasePlayerPawn::GrabRight_Pressed()
{
	GrabObject(RightController);
}

void ABasePlayerPawn::GrabObject(ABaseMotionController* MC)
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (UGlobalFunctionLibrary::IsControllerType(MC, EMotionControllerType::ValveIndex))
			{
				MC->GrabActor();
			}
			else
			{
				if (MC->bWantsToGrip) MC->ReleaseActor();
				else MC->GrabActor();
			}
		}
	}
}

void ABasePlayerPawn::ReleaseObject(ABaseMotionController* MC) const
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (UGlobalFunctionLibrary::IsControllerType(MC, EMotionControllerType::ValveIndex))
			{
				MC->ReleaseActor();
			}
		}
	}
}

// These fire while the trigger is being held and not like a button.
void ABasePlayerPawn::ActivateLeft_Input(const float Val)
{
	if (Val <= UGlobalFunctionLibrary::GetPlayerController(GetWorld())->ThumbstickDeadzone)
	{
		//DeactivateObject(LeftController);
	}
	//ActivateObject(LeftController);
}

void ABasePlayerPawn::ActivateRight_Input(const float Val)
{
	if (Val <= UGlobalFunctionLibrary::GetPlayerController(GetWorld())->ThumbstickDeadzone)
	{
		//DeactivateObject(RightController);
	}
	//ActivateObject(RightController);
}

void ABasePlayerPawn::ActivateObject(ABaseMotionController* MC) const
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (IsValid(MC->AttachedItem))
			{
				if (MC->AttachedItem->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
				{
					const IInteractionInterface* Interface = Cast<IInteractionInterface>(MC->AttachedItem);
					Interface->Execute_Activate(MC->AttachedItem);
				}
			}
		}
	}
}

void ABasePlayerPawn::DeactivateObject(ABaseMotionController* MC) const
{
	if (IsValid(MC))
	{
		if (MC == LeftController || MC == RightController)
		{
			if (IsValid(MC->AttachedItem))
			{
				if (MC->AttachedItem->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
				{
					const IInteractionInterface* Interface = Cast<IInteractionInterface>(MC->AttachedItem);
					Interface->Execute_Deactivate(MC->AttachedItem);
				}
			}
		}
	}
}
