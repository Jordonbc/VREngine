// 


#include "Player/AdvancedPlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogAdvancedPlayer, All, All)

// Sets default values
AAdvancedPlayerPawn::AAdvancedPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	BodyComp->SetupAttachment(VROriginComp);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>BodyObject(TEXT("SkeletalMesh'/VREngine/Meshes/Mannequin/Character_NoHands/SK_Mannequin_NoHands.SK_Mannequin_NoHands'"));
	BodyComp->SetSkeletalMesh(BodyObject.Object);
}

// Called when the game starts or when spawned
void AAdvancedPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	BodyComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, true));

	//UE_LOG(LogAdvancedPlayer, Verbose, TEXT("INFO: Setting timers for IK"));
	//GetWorldTimerManager().SetTimer(IKFootTickFunc, this, &AAdvancedPlayerPawn::IKFootTick, GetInverseKinematicFPS(), true);
	//if (bUseOldTracking) GetWorldTimerManager().SetTimer(UpdateHandIKFunc, this, &AAdvancedPlayerPawn::UpdateHandIK, GetInverseKinematicFPS(), true);
	
}

// Called every frame
void AAdvancedPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//const FVector VecAdd = UKismetMathLibrary::Add_VectorVector(CameraComp->GetComponentLocation(), UKismetMathLibrary::Multiply_VectorFloat(CameraComp->GetForwardVector(), -20));
	//BodyComp->SetWorldLocation(FVector(VecAdd.X, VecAdd.Y, CameraComp->GetComponentLocation().Z - 170), false, nullptr, ETeleportType::TeleportPhysics);
	CalculateMovement(DeltaTime);
}

// Called to bind functionality to input
void AAdvancedPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

