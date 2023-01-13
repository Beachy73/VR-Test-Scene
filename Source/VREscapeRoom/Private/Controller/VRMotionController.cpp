// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/VRMotionController.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Objects/PickupActor.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"

// Sets default values
AVRMotionController::AVRMotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup root component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	// Setup motion controller component
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(RootComp);

	// Finding skeleton mesh for motion controllers
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("/Game/VirtualReality/Mannequin/Character/Mesh/MannequinHand_Right"));
	
	// Setup hand mesh for motion controllers
	ControllerHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	ControllerHandMesh->SetupAttachment(MotionController);
	ControllerHandMesh->SetSkeletalMesh(SkelMesh.Object);
	ControllerHandMesh->SetRelativeLocation(FVector(-10.f, 0.f, 0.f));

	// Setup collision sphere component
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphereSize = 15.f;
	CollisionSphere->SetSphereRadius(CollisionSphereSize);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionSphere->SetupAttachment(MotionController);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AVRMotionController::OnSphereBeginOverlap);

	// Finding haptic effect curve
	static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Curve> GetHapticCurve(TEXT("/Game/Haptics/VRControllerHaptic"));

	// Setup haptic curve
	HapticCurve = GetHapticCurve.Object;
	//HapticBase = CreateDefaultSubobject<UHapticFeedbackEffect_Base>(TEXT("HapticBase"));
	//MotionControllerHaptics->HapticDetails = HapticCurve.Object->HapticDetails;
	

	// Setup default values for variables
	TAG_PickupTag = FName("Pickup");
	bWantsToGrip = false;
	AttachedActor = NULL;
	ActorNearHand = NULL;
	CurrentGripStatus = GripStatus::Open;


	// Teleport relevant setup 
	/*Arc = CreateDefaultSubobject<USplineComponent>(TEXT("Arc"));
	Arc->SetupAttachment(ControllerHandMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Beam("/Game/VirtualReality/Meshes/BeamMesh");
	BeamMesh = Beam.Object;*/
}

// Called when the game starts or when spawned
void AVRMotionController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (ControllerHand == EControllerHand::Right)
	{
		// Setup right hand motion controller
		MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
		MotionController->AttachToComponent(this->RootComponent, AttachRules);

		ControllerHandMesh->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));

		//ControllerHand = EControllerHand::AnyHand;
	}
	else if (ControllerHand == EControllerHand::Left)
	{
		// Setup left hand motion controller
		MotionController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
		MotionController->AttachToComponent(this->RootComponent, AttachRules);

		ControllerHandMesh->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
		ControllerHandMesh->SetRelativeScale3D(FVector(1.f, 1.f, -1.f));

		//ControllerHand = EControllerHand::AnyHand;
	}

	// Teleport
	/*if (Arc)
	{
		ClearArc();
	}
	if (bTeleportActive)
	{
		DrawArc();
	}

	if (!bTeleportActive && HitResult)
	{
		TeleportToLocation();
	}*/
}


void AVRMotionController::Grab()
{
	// Set to true for hand animations
	bWantsToGrip = true;

	// Get actor nearest to hand to pickup
	AActor* cActor = GetActorNearHand();

	if (cActor != NULL)
	{
		APickupActor* pActor = Cast<APickupActor>(cActor);

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, FString::Printf(TEXT("Grabbed actor = %s"), *pActor->GetName()));

		FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		pActor->StaticMesh->AttachToComponent(MotionController, AttachRules);
		pActor->StaticMesh->SetSimulatePhysics(false);
		pActor->SetActorEnableCollision(false);
		AttachedActor = pActor;
	}
}


void AVRMotionController::Release()
{
	// Set to false for hand animations
	bWantsToGrip = false;
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* aActors : AttachedActors)
	{
		if (aActors->GetClass()->IsChildOf(APickupActor::StaticClass()))
		{
			APickupActor* pActor = Cast<APickupActor>(aActors);

			pActor->StaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			pActor->StaticMesh->SetSimulatePhysics(true);
			pActor->SetActorEnableCollision(true);
			AttachedActor = NULL;
		}
	}
}


AActor* AVRMotionController::GetActorNearHand()
{
	// Get actor closest to the position of the motion controller
	
	AActor* cActor = NULL;
	TArray<AActor*> Overlapping;
	CollisionSphere->GetOverlappingActors(Overlapping);

	for (AActor* oActor : Overlapping)
	{
		if (cActor == NULL && oActor->GetClass()->IsChildOf(APickupActor::StaticClass()))
		{
			cActor = oActor;
		}
		else if (oActor->GetClass()->IsChildOf(APickupActor::StaticClass()))
		{
			float CompetingDistance = FVector::Distance(oActor->GetActorLocation(), ControllerHandMesh->GetComponentLocation());
			float OriginalDistance = FVector::Distance(cActor->GetActorLocation(), ControllerHandMesh->GetComponentLocation());

			if (CompetingDistance < OriginalDistance)
			{
				cActor = oActor;
				ActorNearHand = cActor;
			}
		}
	}

	return cActor;
}


void AVRMotionController::RumbleController(float Intensity)
{
	// Vibrate controller with the given intensity
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	EControllerHand EffectHand = ControllerHand;
	UHapticFeedbackEffect_Base* HapticEffect = Cast<UHapticFeedbackEffect_Base>(HapticCurve);

	PC->PlayHapticEffect(HapticEffect, EffectHand, Intensity);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Calling Rumble Controller Function on hand: %s"), EffectHand));
}


void AVRMotionController::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Collision Sphere Overlapping")));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("OtherComp Name = %s"), *OtherComp->GetName()));

		UStaticMeshComponent* OtherMesh = Cast<UStaticMeshComponent>(OtherComp);
		if (OtherMesh)
		{
			if (OtherMesh->IsSimulatingPhysics())
			{
				RumbleController(0.8f);
			}
		}
	}
}


//void AVRMotionController::OnSphereCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Just hit: %s"), *OtherActor->GetName()));
//		
//		float VecLength = NormalImpulse.Size();
//		float Intensity = UKismetMathLibrary::MapRangeClamped(VecLength, 0.f, 1500.f, 0.f, 0.8f);
//		RumbleController(Intensity);
//	}
//}


//void AVRMotionController::DrawArc()
//{
//	FVector ArcLocation = ControllerHandMesh->GetComponentLocation();
//
//	FPredictProjectilePathParams Params = FPredictProjectilePathParams(0.0f, ArcLocation, ControllerHandMesh->GetForwardVector() * ArcVelocity, 2.f);
//	Params.bTraceWithCollision = true;
//	Params.bTraceComplex = false;
//	Params.TraceChannel = ECollisionChannel::ECC_WorldStatic;
//
//	HitResult = UGameplayStatics::PredictProjectilePath(GetWorld(), Params, PredictResult);
//
//	if (HitResult)
//	{
//		for (const FPredictProjectilePathPointData result : PredictResult.PathData)
//		{
//			SplinePoints.Add(result.Location);
//			Arc->AddSplinePoint(result.Location, ESplineCoordinateSpace::World);
//		}
//	}
//
//	if (Arc)
//	{
//		Arc->SetSplinePointType(PredictResult.PathData.Num() - 1, ESplinePointType::CurveClamped);
//
//		for (int32 i = 0; i < SplinePoints.Num() - 2; i++)
//		{
//			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>();
//			SplineMesh->RegisterComponentWithWorld(GetWorld());
//			SplineMesh->SetStaticMesh(BeamMesh);
//			SplineMeshes.Add(SplineMesh);
//
//			FVector Start = SplinePoints[i];
//			FVector End = SplinePoints[i + 1];
//			FVector sTan = FVector::ZeroVector;
//			FVector eTan = FVector::ZeroVector;
//
//			SplineMesh->SetStartAndEnd(Start, sTan, End, eTan);
//		}
//	}
//}
//
//
//void AVRMotionController::ClearArc()
//{
//	Arc->ClearSplinePoints();
//	SplinePoints.empty();
//	
//	for (int32 i = SplineMeshes.Num() - 1; i >= 0; i--)
//	{
//		if (SplineMeshes[i])
//		{
//			SplineMeshes[i].DestroyComponent();
//		}
//	}
//
//	SplineMeshes.empty();
//}
//
//
//void AVRMotionController::TeleportToLocation()
//{
//	GetOwner()->SetWorldLocation(HitResult.Location);
//	HitResult = nullptr;
//}
