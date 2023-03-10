// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreateHapticFeedback.generated.h"

UCLASS()
class VRESCAPEROOM_API ACreateHapticFeedback : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreateHapticFeedback();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Exposing C++")
		static void CreateHapticFeedbackAsset(FString FolderName, FString AssetName);
};
