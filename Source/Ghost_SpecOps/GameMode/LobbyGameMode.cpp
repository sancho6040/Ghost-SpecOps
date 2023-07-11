// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		if (GameState->PlayerArray.Num() >= 3)
		{
			ChangeLevel();
		}
	}
}

void ALobbyGameMode::ChangeLevel()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, TEXT("Change level"));
	}
	bUseSeamlessTravel= true;
	GetWorld()->ServerTravel("/Game/Game/Levels/SnowMap_01");
}
