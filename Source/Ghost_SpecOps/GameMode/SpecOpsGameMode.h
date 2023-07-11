// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Ghost_SpecOps/Player/GhostPlayerController.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "SpecOpsGamemode.generated.h"

class APlayerCharacter;
class AGhostPlayerController;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEliminatedSignature, APlayerCharacter*, EliminatedCharacter);

UCLASS()
class GHOST_SPECOPS_API ASpecOpsGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(APlayerCharacter* ElimCharacter, AGhostPlayerController* VictimController, AGhostPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);

	// FOnPlayerEliminatedSignature OnPlayerEliminatedDelegate;
};
