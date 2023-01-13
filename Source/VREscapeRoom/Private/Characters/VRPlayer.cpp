// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/VRPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/VRMotionController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Initialise capsule component size
	GetCapsuleComponent()->InitCapsuleSize(32.f, 88.f);

	// Setup root scene component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComp->SetupAttachment(GetCapsuleComponent());
	RootComp->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	//SetRootComponent(RootComp);

	// Setup camera root
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComp);

	// Setup VR camera
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(CameraRoot);

	// Set MaxWalkSpeed
	GetCharacterMovement()->MaxWalkSpeed = 240.f;

	// Set motion controller BP to spawn
	static ConstructorHelpers::FObjectFinder<UBlueprint> MotCon(TEXT("/Game/Blueprints/Player/BP_MotionController"));
	SpawnMotionController = MotCon.Object;
}


// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// Set tracking origin for VR headset
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	// Spawn left motion controller
	SpawnLeftController = GetWorld()->SpawnActor(SpawnMotionController->GeneratedClass);
	//LeftController = GetWorld()->SpawnActor<AVRMotionController>();
	LeftController = Cast<AVRMotionController>(SpawnLeftController);
	LeftController->SetOwner(this);
	LeftController->AttachToComponent(RootComp, AttachRules);
	LeftController->ControllerHand = EControllerHand::Left;

	// Spawn right motion controller
	SpawnRightController = GetWorld()->SpawnActor(SpawnMotionController->GeneratedClass);
	//RightController = GetWorld()->SpawnActor<AVRMotionController>();
	RightController = Cast<AVRMotionController>(SpawnRightController);
	RightController->SetOwner(this);
	RightController->AttachToComponent(RootComp, AttachRules);
	RightController->ControllerHand = EControllerHand::Right;
}


// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AVRPlayer::ForwardMovement(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Forward axis value = %f"), Value));
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Headset rotation = %s"), *VRCamera->GetComponentRotation().ToString()));

	if ((Controller) && (Value != 0.0))
	{
		// Forward movement based on VR Camera orientation
		FVector ForwardVec = VRCamera->GetForwardVector();
		AddMovementInput(ForwardVec, Value);
		
		
		
		
		// Forward movement based on root component rotation
		//AVRPlayer::AddMovementInput(GetActorForwardVector(), Value);
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("SHOULD BE MOVING!")));

		/*FRotator RootRot = RootComp->GetComponentRotation();
		FRotator NewRot = FRotator(0, 0, RootRot.Yaw);
		FVector ForwardVec = UKismetMathLibrary::GetForwardVector(NewRot);

		AddMovementInput(ForwardVec, Value);*/
	}
}


void AVRPlayer::RightMovement(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Right axis value = %f"), Value));

	if ((Controller) && (Value != 0.0))
	{
		// Right movement based on VR camera orientation
		FVector RightVec = VRCamera->GetRightVector();
		AddMovementInput(RightVec, Value);
		
		
		
		// Right movement based on root component rotation
		/*FRotator RootRot = RootComp->GetComponentRotation();
		FRotator NewRot = FRotator(0, 0, RootRot.Yaw);
		FVector RightVec = UKismetMathLibrary::GetRightVector(NewRot);

		AddMovementInput(RightVec, Value);*/
	}
}


void AVRPlayer::TurnAround(float Value)
{
	//AVRPlayer::AddControllerYawInput(Value);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Turn axis value = %f"), Value));

	// Turn around the current location of the VR camera, not centred root component
	if ((Controller) && (Value != 0.0))
	{
		FVector RootLoc = RootComp->GetComponentLocation();
		FVector CameraLoc = VRCamera->GetComponentLocation();

		FVector TargetLoc = RootLoc - CameraLoc;
		FVector AxisVector = FVector(0, 0, 1);
		FVector RotatedVector = TargetLoc.RotateAngleAxis(Value, AxisVector);
		
		FVector NewLoc = RotatedVector + CameraLoc;
		FRotator NewRotation = FRotator(0, Value, 0);

		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("New Location = %s"), *NewLoc.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("New Rotation = %s"), *NewRotation.ToString()));

		RootComp->SetWorldLocation(NewLoc);
		RootComp->AddRelativeRotation(NewRotation);
	}
}


// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement inputs
	PlayerInputComponent->BindAxis("Forward", this, &AVRPlayer::ForwardMovement);
	PlayerInputComponent->BindAxis("Right", this, &AVRPlayer::RightMovement);
	PlayerInputComponent->BindAxis("TurnAround", this, &AVRPlayer::TurnAround);

	// Left & right pickup/drop inputs
	PlayerInputComponent->BindAction("LeftGrip", IE_Pressed, this, &AVRPlayer::GrabLeft);
	PlayerInputComponent->BindAction("LeftGrip", IE_Released, this, &AVRPlayer::ReleaseLeft);
	PlayerInputComponent->BindAction("RightGrip", IE_Pressed, this, &AVRPlayer::GrabRight);
	PlayerInputComponent->BindAction("RightGrip", IE_Released, this, &AVRPlayer::ReleaseRight);

	// Controller vibration tests
	PlayerInputComponent->BindAction("VibrationTestLeft", IE_Pressed, this, &AVRPlayer::VibrationTestLeft);
	PlayerInputComponent->BindAction("VibrationTestRight", IE_Pressed, this, &AVRPlayer::VibrationTestRight);


	// Teleport inputs
	/*PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &AVRPlayer::TeleportLeft);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AVRPlayer::TeleportLeftRelease);
	PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &AVRPlayer::TeleportRight);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AVRPlayer::TeleportRightRelease);*/
}


void AVRPlayer::TeleportLeft()
{
	LeftController->bTeleportActive = true;
}


void AVRPlayer::TeleportLeftRelease()
{
	LeftController->bTeleportActive = false;
}


void AVRPlayer::TeleportRight()
{
	RightController->bTeleportActive = true;
}


void AVRPlayer::TeleportRightRelease()
{
	RightController->bTeleportActive = false;
}


void AVRPlayer::GrabLeft()
{
	LeftController->Grab();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Grabbed Left"));
}


void AVRPlayer::ReleaseLeft()
{
	LeftController->Release();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Released Left"));
}


void AVRPlayer::GrabRight()
{
	RightController->Grab();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Grabbed Right"));
}


void AVRPlayer::ReleaseRight()
{
	RightController->Release();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Released Right"));
}


void AVRPlayer::VibrationTestLeft()
{
	LeftController->RumbleController(1.f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Calling the Vibration Test Function on LEFT controller"));
}


void AVRPlayer::VibrationTestRight()
{
	RightController->RumbleController(1.f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Calling the Vibration Test Function on RIGHT controller"));
}

