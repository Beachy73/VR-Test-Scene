// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VRMenu.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AVRMenu::AVRMenu()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneRoot->SetupAttachment(RootComponent);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(SceneRoot);
	WidgetComp->SetDrawSize(FVector2D(1920.f, 1080.f));
	// Set draw size to 120x1080
}

// Called when the game starts or when spawned
void AVRMenu::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

