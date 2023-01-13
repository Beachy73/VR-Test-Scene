// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class UCapsuleComponent;
class USceneComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UMotionControllerComponent;
class UPrimitiveComponent;
class AActor;
class IHeadMountedDisplay;
class UWidgetInteractionComponent;
class AVRMenu;

UCLASS()
class VRESCAPEROOM_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

		UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USceneComponent* VRCameraRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* VRCameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
		UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		UMotionControllerComponent* MC_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		UMotionControllerComponent* MC_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USkeletalMeshComponent* LeftHandMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USkeletalMeshComponent* RightHandMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USceneComponent* LeftHeldObjLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		USceneComponent* RightHeldObjLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
		USceneComponent* MenuLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		UWidgetInteractionComponent* LeftInteractionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
		UWidgetInteractionComponent* RightInteractionComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value);

	void TurnAround(float Value);*/

	void LookUpRate(float Value);

	void LeftPickup();
	void RightPickup();
	void LeftDrop();
	void RightDrop();

	void OpenMenu();
	void CloseMenu();

	void LeftSelect();
	void RightSelect();

	UPROPERTY(EditAnywhere, Category = "VR")
		float LineTraceDistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
		AVRMenu* VRMenu;
	UPROPERTY(EditAnywhere, Category = "Interaction")
		FName TAG_PickupTag;

	bool bIsLeftHoldingObj;
	bool bIsRightHoldingObj;
	bool bWasLeftSimulatingPhysics;
	bool bWasRightSimulatingPhysics;
	UPrimitiveComponent* LeftHitComp;
	UPrimitiveComponent* RightHitComp;
	AActor* LeftPickedUpActor;
	AActor* RightPickedUpActor;
	IHeadMountedDisplay* vrHeadset;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
