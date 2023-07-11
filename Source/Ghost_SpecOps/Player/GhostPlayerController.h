#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Ghost_SpecOps/HUD/PlayerHUD.h"
#include "GhostPlayerController.generated.h"

class APlayerHUD;

UCLASS()
class GHOST_SPECOPS_API AGhostPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHUDHealth(float InHealth, float MaxHealth);
	void SetHUDWeaponAmmo(int32 InAmmo);
	void SetHUDCarriedAmmo(int32 InAmmo);

protected:
	virtual void BeginPlay() override;

private:
	APlayerHUD* PlayerHUD;
};
