// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/VRGameMode.h"
#include "Characters/VRCharacter.h"

AVRGameMode::AVRGameMode()
{
	DefaultPawnClass = AVRCharacter::StaticClass();
}
