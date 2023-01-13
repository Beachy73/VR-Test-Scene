// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/VRCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplay.h"
#include "Components/WidgetInteractionComponent.h"
#include "InputCoreTypes.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VR Root Setup
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetCapsuleComponent());
	VRRoot->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	//SetRootComponent(VRRoot);

	// Camera Root Setup
	VRCameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRCameraRoot"));
	VRCameraRoot->SetupAttachment(VRRoot);

	// Camera Setup
	VRCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCameraComp"));
	VRCameraComp->SetupAttachment(VRCameraRoot);
	VRCameraComp->bUsePawnControlRotation = false;
	VRCameraComp->SetRelativeLocation(FVector(0, 0, 0));

	// Left Motion Controller Setup
	MC_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MC_Left"));
	MC_Left->SetupAttachment(VRRoot);
	MC_Left->bDisplayDeviceModel = false;
	MC_Left->MotionSource = FName(TEXT("Left"));

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(MC_Left);

	LeftHeldObjLoc = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHeldObjLoc"));
	LeftHeldObjLoc->SetupAttachment(MC_Left);
	LeftHeldObjLoc->SetRelativeLocation(FVector(38, 0, 0));

	LeftInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("LeftInteractionComp"));
	LeftInteractionComp->SetupAttachment(MC_Left);
	LeftInteractionComp->InteractionDistance = 750.f;
	LeftInteractionComp->bShowDebug = false;
	LeftInteractionComp->DebugColor = FColor::Cyan;
	LeftInteractionComp->PointerIndex = 0;

	// Right Motion Controller Setup
	MC_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MC_Right"));
	MC_Right->SetupAttachment(VRRoot);
	MC_Right->bDisplayDeviceModel = false;
	MC_Right->MotionSource = FName(TEXT("Right"));

	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(MC_Right);

	RightHeldObjLoc = CreateDefaultSubobject<USceneComponent>(TEXT("RightHeldObjLoc"));
	RightHeldObjLoc->SetupAttachment(MC_Right);
	RightHeldObjLoc->SetRelativeLocation(FVector(38, 0, 0));

	RightInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("RightInteractionComp"));
	RightInteractionComp->SetupAttachment(MC_Right);
	RightInteractionComp->InteractionDistance = 750.f;
	RightInteractionComp->bShowDebug = false;
	RightInteractionComp->DebugColor = FColor::Cyan;
	RightInteractionComp->PointerIndex = 1;

	// Menu Location Setup
	MenuLoc = CreateDefaultSubobject<USceneComponent>(TEXT("MenuLoc"));
	MenuLoc->SetupAttachment(VRCameraComp);
	MenuLoc->SetRelativeLocation(FVector(300.f, 0, 0));
	//MenuLoc->SetUsingAbsoluteRotation(true);

	// Variable Settings
	GetCharacterMovement()->MaxWalkSpeed = 240.f;

	bIsLeftHoldingObj = false;
	bIsRightHoldingObj = false;
	bWasLeftSimulatingPhysics = false;
	bWasRightSimulatingPhysics = false;
	LineTraceDistance = 100.f;
	TAG_PickupTag = FName("Pickup");
}


// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	//bUseControllerRotationPitch = true;
	//bUseControllerRotationYaw = true;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ControllerRotationYaw = %s"), bUseControllerRotationYaw));

	
}


// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (VRMenu != nullptr)
	{
		LeftInteractionComp->bShowDebug = true;
		RightInteractionComp->bShowDebug = true;
	}
	else if (VRMenu == nullptr)
	{
		LeftInteractionComp->bShowDebug = false;
		RightInteractionComp->bShowDebug = false;
	}
}

/*
void AVRCharacter::MoveForward(float Value)
{
	/*if ((Controller) && (Value != 0.0))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}

	if ((Controller) && (Value != 0.0))
	{
		FRotator RootRot = VRRoot->GetComponentRotation();
		FRotator NewRot = FRotator(0, 0, RootRot.Yaw);
		FVector ForwardVec = UKismetMathLibrary::GetForwardVector(NewRot);

		AddMovementInput(ForwardVec, Value);
	}
}


void AVRCharacter::MoveRight(float Value)
{
	/*if ((Controller) && (Value != 0.0))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}

	if ((Controller) && (Value != 0.0))
	{
		FRotator RootRot = VRRoot->GetComponentRotation();
		FRotator NewRot = FRotator(0, 0, RootRot.Yaw);
		FVector RightVec = UKismetMathLibrary::GetRightVector(NewRot);

		AddMovementInput(RightVec, Value);
	}
}


void AVRCharacter::TurnAtRate(float Value)
{
	AVRCharacter::AddControllerYawInput(Value);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Should be turning?"));

	//if ((Controller) && (Value != 0.0))
	//{
		/*FVector RootLoc = VRRoot->GetComponentLocation();
		FVector CameraLoc = VRCameraComp->GetComponentLocation();
		FVector TargetLoc = RootLoc - CameraLoc;
		FVector RotatedVector = TargetLoc.RotateAngleAxis(Value, FVector(0, 0, 1));
		TargetLoc = RotatedVector + CameraLoc;
		FRotator NewRotation = FRotator(0, 0, Value);

		VRRoot->SetWorldLocation(TargetLoc);
		VRRoot->AddRelativeRotation(NewRotation);
	//}
}


void AVRCharacter::TurnAround(float Value)
{
	AddControllerYawInput(Value);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Should be turning?"));
}

void AVRCharacter::LookUpRate(float Value)
{
	AVRCharacter::AddControllerPitchInput(Value);
}


void AVRCharacter::LeftPickup()
{
	FVector Start = MC_Left->GetComponentLocation();
	FVector ForwardVec = MC_Left->GetForwardVector();
	FVector End = ForwardVec * LineTraceDistance;
	End = End + Start;

	FCollisionQueryParams TraceParams;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	//FCollisionObjectQueryParams ObjectParams(ECC_PhysicsBody);

	FHitResult Hit = FHitResult(ForceInit);
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	//bool bIsHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, TraceParams);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RUNNING LINE TRACE"));

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

	if (bIsHit && Hit.GetActor()->ActorHasTag(TAG_PickupTag))
	{
		LeftHitComp = Hit.GetComponent();
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("LINE TRACE SUCCESSFUL"));

		if (LeftHitComp->IsSimulatingPhysics() == true)
		{
			LeftHitComp->SetSimulatePhysics(false);
			bWasLeftSimulatingPhysics = true;
		}
		LeftPickedUpActor = Hit.GetActor();
		LeftPickedUpActor->AttachToComponent(LeftHeldObjLoc, AttachRules);
		bIsLeftHoldingObj = true;
	}
}


void AVRCharacter::RightPickup()
{
	FVector Start = MC_Right->GetComponentLocation();
	FVector ForwardVec = MC_Right->GetForwardVector();
	FVector End = ForwardVec * LineTraceDistance;
	End = End + Start;

	FCollisionQueryParams TraceParams;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	//FCollisionObjectQueryParams ObjectParams(ECC_PhysicsBody);

	FHitResult Hit = FHitResult(ForceInit);
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	//bool bIsHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, TraceParams);
	//bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_EngineTraceChannel1, TraceParams);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RUNNING LINE TRACE"));

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
	

	if (bIsHit && Hit.GetActor()->ActorHasTag(TAG_PickupTag))
	{
		RightHitComp = Hit.GetComponent();

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("LINE TRACE SUCCESSFUL"));

		if (RightHitComp->IsSimulatingPhysics() == true)
		{
			RightHitComp->SetSimulatePhysics(false);
			bWasRightSimulatingPhysics = true;
		}
		RightPickedUpActor = Hit.GetActor();
		RightPickedUpActor->AttachToComponent(RightHeldObjLoc, AttachRules);
		bIsRightHoldingObj = true;
	}
}


void AVRCharacter::LeftDrop()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	
	if (IsValid(LeftPickedUpActor))
	{
		LeftPickedUpActor->DetachFromActor(DetachRules);
		
		if (IsValid(LeftHitComp))
		{
			if (bWasLeftSimulatingPhysics)
			{
				LeftHitComp->SetSimulatePhysics(true);
				bWasLeftSimulatingPhysics = false;
			}
			bIsLeftHoldingObj = false;
			LeftHitComp = nullptr;
			LeftPickedUpActor = nullptr;
		}
	}
}


void AVRCharacter::RightDrop()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);

	if (IsValid(RightPickedUpActor))
	{
		RightPickedUpActor->DetachFromActor(DetachRules);

		if (IsValid(RightHitComp))
		{
			if (bWasRightSimulatingPhysics)
			{
				RightHitComp->SetSimulatePhysics(true);
				bWasRightSimulatingPhysics = false;
			}
			bIsRightHoldingObj = false;
			RightHitComp = nullptr;
			RightPickedUpActor = nullptr;
		}
	}
}

*/
void AVRCharacter::OpenMenu()
{
	//FVector Loc = FVector(0, 0, 0);
	//FRotator Rot = FRotator(0, 0, 0);
	//FTransform Transform = FTransform(Loc);
	//FActorSpawnParameters SpawnInfo;
	////AActor* VRMenu = GetWorld()->SpawnActor<AActor>();
	////GetWorld->SpawnActor(VRMenu<AActor>(), Loc, Rot, SpawnInfo);
	//GetWorld()->SpawnActor<AVRMenu>(Loc, Rot, SpawnInfo);
	////GetWorld()->SpawnActor(VRMenu, Transform, SpawnInfo);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Turquoise, TEXT("Opening Menu!"));
}


void AVRCharacter::CloseMenu()
{

}


void AVRCharacter::LeftSelect()
{
	
	/*FKey* LeftMouseButton;
	LeftMouseButton = FKey::FName(TEXT("Left Mouse Button"));
	LeftInteractionComp->PressPointerKey();*/
}


void AVRCharacter::RightSelect()
{

}


// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Player Movement
	//PlayerInputComponent->BindAxis("MoveForward", this, &AVRCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AVRCharacter::MoveRight);

	// Look Controls
	//PlayerInputComponent->BindAxis("Turn", this, &AVRCharacter::TurnAtRate);
	////PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AVRCharacter::LookUpRate);

	////PlayerInputComponent->BindAxis("Turn", this, &AVRCharacter::TurnAround);

	// Grip Controls
	//PlayerInputComponent->BindAction("LeftGrip", IE_Pressed, this, &AVRCharacter::LeftPickup);
	//PlayerInputComponent->BindAction("LeftGrip", IE_Released, this, &AVRCharacter::LeftDrop);
	//PlayerInputComponent->BindAction("RightGrip", IE_Pressed, this, &AVRCharacter::RightPickup);
	//PlayerInputComponent->BindAction("RightGrip", IE_Released, this, &AVRCharacter::RightDrop);

	// Open/Close Menu
	PlayerInputComponent->BindAction(TEXT("OpenMenu"), IE_Pressed, this, &AVRCharacter::OpenMenu);
	PlayerInputComponent->BindAction(TEXT("OpenMenu"), IE_Released, this, &AVRCharacter::CloseMenu);

	// Menu Selection
	PlayerInputComponent->BindAction(TEXT("LeftSelect"), IE_Pressed, this, &AVRCharacter::LeftSelect);
	PlayerInputComponent->BindAction(TEXT("RightSelect"), IE_Pressed, this, &AVRCharacter::RightSelect);

	
}

