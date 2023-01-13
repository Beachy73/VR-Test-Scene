// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/VRMotionController.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->InitCapsuleSize(32.f, 88.f);
	SetRootComponent(CapsuleComp);

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(CapsuleComp);
	VROrigin->SetRelativeLocation(FVector(0, 0, -CapsuleComp->GetScaledCapsuleHalfHeight()));

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VROrigin);

	MoveSpeed = 2.f;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	LeftController = GetWorld()->SpawnActor<AVRMotionController>();
	LeftController->SetOwner(this);
	LeftController->AttachToComponent(VROrigin, AttachRules);
	LeftController->ControllerHand = EControllerHand::Left;

	RightController = GetWorld()->SpawnActor<AVRMotionController>();
	RightController->SetOwner(this);
	RightController->AttachToComponent(VROrigin, AttachRules);
	RightController->ControllerHand = EControllerHand::Right;
}


// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APlayerPawn::MoveForward(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Forward axis value = %f"), Value));
	
	if ((Controller) && (Value != 0.0))
	{
		FVector ForwardVec = VRCamera->GetForwardVector();
		FVector NewVec = UKismetMathLibrary::ProjectVectorOnToPlane(ForwardVec, FVector(0, 0, 1));
		NewVec = NewVec * Value * MoveSpeed;

		AddActorWorldOffset(NewVec, true);
	}
}


void APlayerPawn::MoveRight(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("Right axis value = %f"), Value));

	if ((Controller) && (Value != 0.0))
	{
		FVector RightVec = VRCamera->GetRightVector();
		FVector NewVec = UKismetMathLibrary::ProjectVectorOnToPlane(RightVec, FVector(0, 0, 1));
		NewVec = NewVec * Value * MoveSpeed;

		AddActorWorldOffset(NewVec, true);
	}
}


void APlayerPawn::Turn(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Turn axis value = %f"), Value));

	if ((Controller) && (Value != 0.0))
	{
		/*FRotator NewRot = FRotator(0, Value, 0);
		AddActorWorldRotation(NewRot);*/


		FVector OriginLoc = VROrigin->GetComponentLocation();
		FVector CameraLoc = VRCamera->GetComponentLocation();

		FVector TargetLoc = OriginLoc - CameraLoc;
		FVector AxisVector = FVector(0, 0, 1);
		FVector RotatedVector = TargetLoc.RotateAngleAxis(Value, AxisVector);

		FVector NewLoc = RotatedVector + CameraLoc;
		FRotator NewRotation = FRotator(0, Value, 0);

		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("New Location = %s"), *NewLoc.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("New Rotation = %s"), *NewRotation.ToString()));

		VROrigin->SetWorldLocation(NewLoc);
		VROrigin->AddRelativeRotation(NewRotation);
	}
}


// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PlayerForward", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("PlayerRight", this, &APlayerPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerPawn::Turn);
}

