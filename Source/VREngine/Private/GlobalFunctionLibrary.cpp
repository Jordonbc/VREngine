// Copyright 2017-2020 Jordon Brooks.


#include "GlobalFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Player/VRPlayerController.h"
//#include "Log.h"
#include "IMotionController.h"

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
	return (GEditor->bIsSimulatingInEditor || (GEditor->PlayWorld != NULL));
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