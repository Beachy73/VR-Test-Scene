// Fill out your copyright notice in the Description page of Project Settings.


#include "Vibration/CreateHapticFeedback.h"
#include "AssetRegistryModule.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"

// Sets default values
ACreateHapticFeedback::ACreateHapticFeedback()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACreateHapticFeedback::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACreateHapticFeedback::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACreateHapticFeedback::CreateHapticFeedbackAsset(FString FolderName, FString AssetName)
{
	// Create package to save asset into
	FString FullPackageName = "/Game/" + FolderName / AssetName;
	UPackage* Package = CreatePackage(nullptr, *FullPackageName);
	Package->FullyLoad();

	// Create asset in the new package
	UHapticFeedbackEffect_Curve* AssetPtr = NewObject<UHapticFeedbackEffect_Curve>((UObject*)Package, FName(*AssetName), RF_Public | RF_Standalone);
	FAssetRegistryModule::AssetCreated(AssetPtr);
	AssetPtr->MarkPackageDirty();
}