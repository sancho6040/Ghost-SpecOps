#include "PlayerCombatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Ghost_SpecOps/BaseCharacter/BaseCharacter.h"
#include "Ghost_SpecOps/HUD/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Hearing.h"


UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UPlayerCombatComponent, CombatState);
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(PlayerCharacter)
	{
		if(PlayerCharacter->GetFollowCamera())
		{
			DefaulfFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaulfFOV;
		}
		if (PlayerCharacter->HasAuthority())
		{
			InitializeCarriedAmmo();
		}

		// if(PlayerCharacter->GetWeapon())
		// {
		// 	if(CarriedAmmoMap.Contains(PlayerCharacter->GetWeapon()->GetWeaponType()))
		// 	{
		// 		CarriedAmmo = CarriedAmmoMap[PlayerCharacter->GetWeapon()->GetWeaponType()];
		// 	}
		// }

		CarriedAmmo = StartingARAmmo;
		
		Controller = Controller == nullptr ? Cast<AGhostPlayerController>(PlayerCharacter->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDCarriedAmmo(CarriedAmmo);
		}
	}
	
}

void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UPlayerCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);
		CrosshairShootingFactor += 0.75f;
		CrosshairShootingFactor = FMath::Clamp(CrosshairShootingFactor, 0.f, 1.f);

		StartFireTimer();
	}
}

void UPlayerCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UPlayerCombatComponent::Reload()
{
	if(CarriedAmmo > 0 && CombatState != ECombatStates::ECS_Reloading)
	{
		Server_Reload();		
	}
}

void UPlayerCombatComponent::FinishReload()
{
	if(PlayerCharacter == nullptr)
	{
		return;
	}
	if(PlayerCharacter->HasAuthority())
	{
		CombatState = ECombatStates::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if(bFireButtonPressed)
	{
		Fire();
	}
}

void UPlayerCombatComponent::UpdateAmmoValues()
{
	if(!PlayerCharacter || !PlayerCharacter->GetWeapon())
	{
		return;
	}
	//update CarriedAmmo
	const int32 ReloadAmount = AmountToReload();
	if(CarriedAmmoMap.Contains(PlayerCharacter->GetWeapon()->GetWeaponType()))
	{
		CarriedAmmoMap[PlayerCharacter->GetWeapon()->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[PlayerCharacter->GetWeapon()->GetWeaponType()];
	}
	//update HUD
	Controller = Controller == nullptr ? Cast<AGhostPlayerController>(PlayerCharacter->Controller) : Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	//add ammo to the weapon
	PlayerCharacter->GetWeapon()->AddAmmo(-ReloadAmount);
}

void UPlayerCombatComponent::Server_Reload_Implementation()
{
	if(!PlayerCharacter || !PlayerCharacter->GetWeapon())
	{
		return;
	}

	CombatState = ECombatStates::ECS_Reloading;
	HandleReload();
}

void UPlayerCombatComponent::HandleReload()
{
	PlayerCharacter->PlayReloadMontage();

}

int32 UPlayerCombatComponent::AmountToReload()
{
	if(!PlayerCharacter || !PlayerCharacter->GetWeapon())
	{
		return 0;
	}
	const AWeapon* EquippedWeapon = PlayerCharacter->GetWeapon();
	const int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		const int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		const int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0 , Least);
	}
	return 0;
}

void UPlayerCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
		case ECombatStates::ECS_Reloading:
			HandleReload();
			break;
		case ECombatStates::ECS_Unoccupied:
			if (bFireButtonPressed)
			{
				Fire();
			}
			break;
	}
}

void UPlayerCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& InTraceHitTarget)
{
	MulticastFire(InTraceHitTarget);
}

void UPlayerCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& InTraceHitTarget)
{
	if (PlayerCharacter->GetWeapon() == nullptr) return;
	if (PlayerCharacter && CombatState == ECombatStates::ECS_Unoccupied)
	{
		PlayerCharacter->PlayFireMontage(PlayerCharacter->bIsAiming);
		PlayerCharacter->GetWeapon()->Fire(InTraceHitTarget);

		UAISense_Hearing::ReportNoiseEvent(this, GetOwner()->GetActorLocation(), 1.f, GetOwner(), 1000.f);
	}
}

void UPlayerCombatComponent::StartFireTimer()
{
	if(PlayerCharacter && PlayerCharacter->GetWeapon() )
	{
		PlayerCharacter->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UPlayerCombatComponent::FinishFireTimer,
			PlayerCharacter->GetWeapon()->FireDelay
		);
	}
}

void UPlayerCombatComponent::FinishFireTimer()
{
	bCanFire = true;
	if(bFireButtonPressed && PlayerCharacter->GetWeapon()->bIsAutomatic)
	{
		Fire();
	}
	if(PlayerCharacter->GetWeapon()->IsEmpty())
	{
		Reload();
	}
}

void UPlayerCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(!PlayerCharacter || !PlayerCharacter->Controller)
	{
		return;
	}
	
	Controller = Controller == nullptr ? Cast<AGhostPlayerController>(PlayerCharacter->Controller) : Controller;
	if(Controller)
	{
		HUD = HUD == nullptr ? Cast<APlayerHUD>(Controller->GetHUD()) : HUD;
		if(HUD)
		{
			FHUDPackage HUDPackage;
			if(PlayerCharacter->GetWeapon() && !PlayerCharacter->bIsADS)
			{
				HUDPackage.CrosshairsCenter = PlayerCharacter->GetWeapon()->CrosshairsCenter;
				HUDPackage.CrosshairsTop = PlayerCharacter->GetWeapon()->CrosshairsTop;
				HUDPackage.CrosshairsRight = PlayerCharacter->GetWeapon()->CrosshairsRight;
				HUDPackage.CrosshairsBottom = PlayerCharacter->GetWeapon()->CrosshairsBottom;
				HUDPackage.CrosshairsLeft = PlayerCharacter->GetWeapon()->CrosshairsLeft;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
			}

			//Calculate CrosshairSpread
			FVector2d WalkSpeedRange(0.f, PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2d VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = PlayerCharacter->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if(PlayerCharacter->bIsAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;
			
			HUD->SetHudPackage(HUDPackage);

		}
	}
	
}

void UPlayerCombatComponent::InterpFOV(float DeltaTime)
{
	if(PlayerCharacter->GetWeapon())
	{
		if(PlayerCharacter->bIsAiming && !PlayerCharacter->bIsADS)
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, PlayerCharacter->GetWeapon()->GetZoomedFOV(), DeltaTime, PlayerCharacter->GetWeapon()->GetZoomInterpSpeed());			
		}
		else
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaulfFOV, DeltaTime, ZoomInterpSpeed);			
		}

		if(PlayerCharacter && PlayerCharacter->GetFollowCamera())
		{
			PlayerCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
		}
	}
}

void UPlayerCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			// HitTarget = End;
		}
		else
		{
			// DrawDebugSphere(
			// 	GetWorld(),
			// 	TraceHitResult.ImpactPoint,
			// 	12.f,
			// 	12,
			// 	FColor::Red
			// );
		}
	}

}

bool UPlayerCombatComponent::CanFire()
{
	if(PlayerCharacter && PlayerCharacter->GetWeapon())
	{
		return !PlayerCharacter->GetWeapon()->IsEmpty() && bCanFire && CombatState == ECombatStates::ECS_Unoccupied;
	}
	return false;
}

void UPlayerCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AGhostPlayerController>(Controller) : Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UPlayerCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponTypes::EWT_AssaultRifle, StartingARAmmo);
}


