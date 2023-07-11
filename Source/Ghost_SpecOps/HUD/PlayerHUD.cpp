#include "PlayerHUD.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2d ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2d Spread = FVector2d::Zero();
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2d Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2d Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2d Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2d Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
	}
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void APlayerHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if(PlayerController)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void APlayerHUD::DrawCrosshair(UTexture2D* InTexture, FVector2d InViewportCenter, FVector2d InSpread)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();
	const FVector2d TextureDrawPoint(
		InViewportCenter.X - (TextureWidth / 2.f) + InSpread.X,
		InViewportCenter.Y - (TextureHeight / 2.f) + InSpread.Y
	);

	DrawTexture(
		InTexture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}
