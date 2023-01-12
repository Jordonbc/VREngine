// Copyright 2017-2022 Jordon Brooks.


#include "Player/VRPlayerController.h"
#include "Player/BaseMotionController.h"

AVRPlayerController::AVRPlayerController()
{
	HandControllerClass = ABaseMotionController::StaticClass();
}