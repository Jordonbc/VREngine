// Copyright 2017-2020 Jordon Brooks.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.generated.h"

UENUM(BlueprintType)
enum class EGrabType : uint8
{
	AttachTo 	UMETA(DisplayName = "AttachTo"),
	Physics 	UMETA(DisplayName = "Physics"),
	Mixed	UMETA(DisplayName = "Mixed")
};

UENUM(BlueprintType)
enum class EMotionControllerType : uint8
{
	ValveIndex 	UMETA(DisplayName = "Valve Index"),
	HTCVive 	UMETA(DisplayName = "HTC Vive"),
	Oculus	UMETA(DisplayName = "Oculus"),
	WindowsMixedReality	UMETA(DisplayName = "Windows Mixed Reality")
};

UENUM(BlueprintType)
enum class EMoveMode : uint8
{
	Teleport 	UMETA(DisplayName = "Teleport"),
	Locomotion 	UMETA(DisplayName = "Locomotion"),
};

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
	Camera 	UMETA(DisplayName = "Camera"),
	Controller 	UMETA(DisplayName = "Controller"),
};

UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward 	UMETA(DisplayName = "Forward"),
	Right 	UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class EControllerSelection : uint8
{
	Left 	UMETA(DisplayName = "Left"),
	Right 	UMETA(DisplayName = "Right"),
	Both 	UMETA(DisplayName = "Both"),
};

UENUM(BlueprintType)
enum class EControllerPhysicsType : uint8
{
	Stop 	UMETA(DisplayName = "Stop"),
	Physics 	UMETA(DisplayName = "Physics"),
	NoCollision 	UMETA(DisplayName = "No Collision"),
};
