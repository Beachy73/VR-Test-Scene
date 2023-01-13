// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/BuzzHook.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABuzzHook::ABuzzHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComp->SetupAttachment(RootComponent);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComp);
	
}

// Called when the game starts or when spawned
void ABuzzHook::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuzzHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

