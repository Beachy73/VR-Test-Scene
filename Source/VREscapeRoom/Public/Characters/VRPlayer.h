// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRPlayer.generated.h"

class USceneComponent;
class UCameraComponent;
class AVRMotionController;

UCLASS()
class VRESCAPEROOM_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Reality")
		USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Reality")
		USceneComponent* CameraRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* VRCamera;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AVRMotionController* LeftController;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AActor* SpawnLeftController;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AVRMotionController* RightController;
	UPROPERTY(VisibleAnywhere, Category = "Controller")
		AActor* SpawnRightController;

	UBlueprint* SpawnMotionController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void ForwardMovement(float Value);
	void RightMovement(float Value);
	void TurnAround(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TeleportLeft();
	void TeleportLeftRelease();
	void TeleportRight();
	void TeleportRightRelease();

	void GrabLeft();
	void ReleaseLeft();
	void GrabRight();
	void ReleaseRight();

	void VibrationTestLeft();
	void VibrationTestRight();
};
