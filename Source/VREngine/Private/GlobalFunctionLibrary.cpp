// Copyright 2017-2022 Jordon Brooks.


#include "GlobalFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Player/BaseMotionController.h"
#include "Player/VRPlayerController.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#endif

FName UGlobalFunctionLibrary::GetMotionControllerDeviceTypeName(UMotionControllerComponent* MotionControllerComponent)
{

	return FName();
}

bool UGlobalFunctionLibrary::IsEditorSimulating()
{

#if WITH_EDITOR
	return true;
#else
	return false; // always return false if not using editor
#endif
}

AVRPlayerController* UGlobalFunctionLibrary::GetPlayerController(UWorld* WorldContextObject)
{
	AVRPlayerController* PlayerController = Cast<AVRPlayerController>(WorldContextObject->GetFirstPlayerController());
	//if (!IsValid(PlayerController)) ULog::Fatal("Player controller is not derived from AVRPlayerController or cannot access variable 'HandControllerClass'!");
	return PlayerController;
}

bool UGlobalFunctionLibrary::IsControllerType(const ABaseMotionController* MC, const EMotionControllerType MCType)
{
	return MC->ControllerType == MCType;
}