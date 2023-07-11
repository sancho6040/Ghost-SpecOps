#include "SpecOpsGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Ghost_SpecOps/BaseCharacter/BaseCharacter.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void ASpecOpsGameMode::PlayerEliminated(APlayerCharacter* ElimCharacter, AGhostPlayerController* VictimController, AGhostPlayerController* AttackerController)
{
	if(ElimCharacter)
	{
		ElimCharacter->Die();
	}

	// OnPlayerEliminatedDelegate.Broadcast(ElimCharacter);
}

void ASpecOpsGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if(EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}

	if(EliminatedController)
	{

		//restart the Player to an random plaeyerstart
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}
