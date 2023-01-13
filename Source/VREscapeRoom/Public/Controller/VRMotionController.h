// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRMotionController.generated.h"

class USceneComponent;
class UMotionControllerComponent;
class USkeletalMeshComponent;
class USphereComponent;
class USplineComponent;
class UStaticMesh;
class USplineMeshComponent;
class UHapticFeedbackEffect_Base;
class UHapticFeedbackEffect_Curve;
//struct FPredictProjectilePathResult;

// Grip Status Enum
UENUM()
enum GripStatus
{
	Open		UMETA(DisplayName = "Open"),
	CanGrab		UMETA(DisplayName = "CanGrab"),
	Grab		UMETA(DisplayName = "Grab"),
};

UCLASS()
class VRESCAPEROOM_API AVRMotionController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRMotionController();

	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		UMotionControllerComponent* MotionController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Controller")
		USkeletalMeshComponent* ControllerHandMesh;
	UPROPERTY(VisibleAnywhere, Category = "Motion Controller")
		USphereComponent* CollisionSphere;
	UPROPERTY(VisibleAnywhere, Category = "Teleportation")
		USplineComponent* Arc;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Arc Teleport Variables
	/*FPredictProjectilePathResult PredictResult;
	bool HitResult;
	UStaticMesh* BeamMesh;
	TArray<USplineMeshComponent*> SplineMeshes;
	TArray<FVector> SplinePoints;
	float ArcVelocity = 900.f;*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////////////////////////////////
	///					        	Functions							   ///
	//////////////////////////////////////////////////////////////////////////

	// Grab functions
	void Grab();
	void Release();
	UFUNCTION(BlueprintCallable)
		AActor* GetActorNearHand();

	// Teleport functions (WIP)
	void DrawArc();
	void ClearArc();
	void TeleportToLocation();

	// Vibration Functions
	void RumbleController(float Intensity);

	UFUNCTION()
		void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//////////////////////////////////////////////////////////////////////////
	///					        	Variables							   ///
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grab")
		bool bWantsToGrip;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grab")
		bool bCanGrab;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grab")
		bool bIsGrabbing;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grab")
		AActor* AttachedActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grab")
		AActor* ActorNearHand;
	UPROPERTY(EditAnywhere, Category = "Interaction")
		FName TAG_PickupTag;
	UPROPERTY(EditAnywhere, Category = "Interaction")
		float CollisionSphereSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grip Status")
		TEnumAsByte<GripStatus> CurrentGripStatus;
	

	EControllerHand ControllerHand;

	bool bTeleportActive;

	// Vibration feedback variables
	UHapticFeedbackEffect_Base* HapticBase;
	UHapticFeedbackEffect_Curve* HapticCurve;
};
