// Copyright 2017-2022 Jordon Brooks.

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
	ValveIndex 			UMETA(DisplayName = "Valve Index"),
	HTCVive 			UMETA(DisplayName = "HTC Vive"),
	HTCViveCosmos 			UMETA(DisplayName = "HTC Vive Cosmos"),
	MagicLeapOne				UMETA(DisplayName = "Magic Leap One"),
	HTCViveFocus				UMETA(DisplayName = "HTC Vive Focus"),
	HTCViveFocusPlus				UMETA(DisplayName = "HTC Vive Focus Plus"),
	OculusTouch				UMETA(DisplayName = "Oculus"),
	OculusTouch_v3				UMETA(DisplayName = "OculusTouch v3"),
	OculusTouch_v2				UMETA(DisplayName = "OculusTouch v2"),
	WindowsMixedReality	UMETA(DisplayName = "Windows Mixed Reality"),
	HPMixedReality	UMETA(DisplayName = "HP Mixed Reality"),
	SamsungOdyssey	UMETA(DisplayName = "Samsung Odyssey"),
	SamsungGear	UMETA(DisplayName = "Samsung Gear"),
	PicoNeo	UMETA(DisplayName = "Pico Neo / Pico Neo 2"),
	PicoG2	UMETA(DisplayName = "Pico G2"),
	GoogleDaydream	UMETA(DisplayName = "Google Daydream"),
	NoController		UMETA(DisplayName = "No Controller"), // Unlikely this will get triggered
	Unknown				UMETA(DisplayName = "Unknown")
};

UENUM(BlueprintType)
enum class EMoveMode : uint8
{
	Teleport 	UMETA(DisplayName = "Teleport",  ToolTip = "The classic VR movement."),
	Locomotion 	UMETA(DisplayName = "Locomotion"),
	LocomotionWithZ 	UMETA(DisplayName = "Locomotion with flying", ToolTip = "The classic spectator controls."),
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
	Stop 	UMETA(DisplayName = "Stop", ToolTip = "Legacy VREngine setting, most tested but expensive and runs on tick!"),
	Physics 	UMETA(DisplayName = "Physics", ToolTip = "Hand physics handled by SetWorldLocationAndRotation function. Doesn't always work!"),
	NoCollision 	UMETA(DisplayName = "No Collision", ToolTip = "The traditional VR experience (hand goes through objects)."),
	PhysicsConstraint 	UMETA(DisplayName = "PhysicsConstraint", ToolTip = "BETA! Uses optimized physics constraints to handle hand physics."),
	Custom 	UMETA(DisplayName = "Custom", ToolTip = "Tells VR Engine to not handle hand movement. Use your own function to control HandMeshComponent!"),
};
