// 


#include "Components/VRCamera.h"
#include "HeadMountedDisplayFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogVRCamera, All, All)

// Sets default values for this component's properties
UVRCamera::UVRCamera()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
}


// Called when the game starts
void UVRCamera::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority) bHasAuthority = true;
	else bHasAuthority = false;
	bIsServer = IsRunningDedicatedServer();
	
}

void UVRCamera::Net_RPC_ClientSendTracking_Implementation(FTransform TrackingData)
{
	UE_LOG(LogVRCamera, VeryVerbose, TEXT("Server Recieved Data!"))
	LastPosition = TrackingData;
	SetRelativeRotation(LastPosition.GetRotation());
	SetRelativeLocation(LastPosition.GetLocation());
	
}

void UVRCamera::ServerReplicateTracking(FTransform TrackingData)
{
	
}

// Called every frame
void UVRCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GetOwnerRole() == ROLE_Authority) bHasAuthority = true;
	else bHasAuthority = false;

	TickUpdateTracking(DeltaTime);
}

void UVRCamera::TickUpdateTracking(float DeltaTime)
{
	if (bHasAuthority)
	{
		//const FTransform InterpTransform = UKismetMathLibrary::TInterpTo(GetRelativeTransform(), LastPosition, GetWorld()->DeltaTimeSeconds, 10);
		//SetRelativeTransform(GetRelativeTransform() + LastPosition);
		//SetWorldLocation()
		//SetWorldRotation(LastPosition.GetRotation());
		//UE_LOG(LogVRCamera, VeryVerbose, TEXT("Server Moved! %s"), static_cast<FText>(LastPosition.ToString()))
		FString ConsoleText("Coordinates Recieved: ");
		ConsoleText.Append(LastPosition.ToString());
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Cyan, ConsoleText);
	}
	else
	{
		FRotator TempRotator;
		FVector TempVector;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(TempRotator, TempVector);
		SetRelativeRotation(TempRotator);
		SetRelativeLocation(TempVector);
		Net_RPC_ClientSendTracking(FTransform(TempRotator, TempVector ,FVector(1.0f, 1.0f, 1.0f)));
		
		
	}
}
