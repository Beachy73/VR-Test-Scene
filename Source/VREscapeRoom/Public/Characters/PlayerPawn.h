// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCapsuleComponent;
class USceneComponent;
class UCameraComponent;
class AVRMotionController;

UCLASS()
class VRESCAPEROOM_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Reality")
		UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Reality")
		USceneComponent* VROrigin;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		UCameraComponent* VRCamera;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AVRMotionController* LeftController;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AVRMotionController* RightController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float MoveSpeed;

};
