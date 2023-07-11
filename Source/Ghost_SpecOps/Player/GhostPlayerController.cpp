#include "GhostPlayerController.h"

#include "Components/ProgressBar.h"
#include "Ghost_SpecOps/HUD/CharacterOverlay.h"

void AGhostPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = Cast<APlayerHUD>(GetHUD());
}

void AGhostPlayerController::SetHUDHealth(float InHealth, float MaxHealth)
{
	PlayerHUD = PlayerHUD == nullptr ? Cast<APlayerHUD>(GetHUD()) : PlayerHUD;

	bool bIsHUDValid = PlayerHUD &&
		PlayerHUD->CharacterOverlay &&
		PlayerHUD->CharacterOverlay->HealthBar &&
		PlayerHUD->CharacterOverlay->HealthText;
	
	if(bIsHUDValid)
	{
		const float HealthPercent = InHealth / MaxHealth;
		PlayerHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(MaxHealth));
		PlayerHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AGhostPlayerController::SetHUDWeaponAmmo(int32 InAmmo)
{
	PlayerHUD = PlayerHUD == nullptr ? Cast<APlayerHUD>(GetHUD()) : PlayerHUD;

	bool bIsHUDValid = PlayerHUD &&
		PlayerHUD->CharacterOverlay &&
		PlayerHUD->CharacterOverlay->WeaponAmmoAmount;
	if(bIsHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), InAmmo);
		PlayerHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AGhostPlayerController::SetHUDCarriedAmmo(int32 InAmmo)
{
	PlayerHUD = PlayerHUD == nullptr ? Cast<APlayerHUD>(GetHUD()) : PlayerHUD;

	bool bIsHUDValid = PlayerHUD &&
		PlayerHUD->CharacterOverlay &&
		PlayerHUD->CharacterOverlay->CarriedAmmoAmount;
	if(bIsHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), InAmmo);
		PlayerHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}


