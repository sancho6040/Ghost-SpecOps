#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ghost_SpecOps/Player/GhostPlayerController.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "Ghost_SpecOps/Types/CombatStates.h"
#include "Ghost_SpecOps/Types/WeaponTypes.h"
#include "PlayerCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;
class ABaseCharacter;
class AGhostPlayerController;
class APlayerHUD;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GHOST_SPECOPS_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCombatComponent();

	friend class APlayerCharacter;
	friend class ABaseCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void FireButtonPressed(bool bInState);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReload();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& InTraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& InTraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	void HandleReload();

	int32 AmountToReload();

private:
	
	APlayerCharacter* PlayerCharacter;
	AGhostPlayerController* Controller;
	APlayerHUD* HUD;

	bool bFireButtonPressed;

	// HUD && crosshairs
	float CrosshairVelocityFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	void Fire();

	/*
	 * Aiming && FOV
	 */

	float DefaulfFOV;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*
	 *	Automatic fire 
	 */

	FTimerHandle FireTimer;

	bool bCanFire = true;

	void StartFireTimer();
	void FinishFireTimer();

	bool CanFire();

	//CarriedAmmo for the current equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponTypes, int32> CarriedAmmoMap;
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatStates CombatState = ECombatStates::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	
};
