#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UTexture2D;
class UCharacterOverlay;
class UUserWidget;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsBottom;
	UTexture2D* CrosshairsLeft;

	float CrosshairSpread;
};

UCLASS()
class GHOST_SPECOPS_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	
	UCharacterOverlay* CharacterOverlay;

	FORCEINLINE void SetHudPackage(const FHUDPackage& InPackage) { HUDPackage = InPackage; }

protected:
	virtual void BeginPlay() override;

	void AddCharacterOverlay();

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* InTexture, FVector2d InViewportCenter, FVector2D InStread);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
};
