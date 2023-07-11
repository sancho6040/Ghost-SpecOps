#pragma once
#include "CoreMinimal.h"
#include "Ghost_SpecOps/Weapon/Weapon.h"
#include "GameFramework/Character.h"
#include "Ghost_SpecOps/Types/CombatStates.h"
#include "Ghost_SpecOps/Types/TurningInPlace.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UPlayerCombatComponent;

UCLASS()
class GHOST_SPECOPS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void PlayFireMontage(bool bInAiming) const;
	void PlayReloadMontage() const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsProne;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsAiming;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsADS;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsStanding;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsRunning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DEFAULTS, Replicated)
	bool bIsAlive;

protected:
	virtual void BeginPlay() override;
	//---------------------------------- Character Weapon settings ---------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TSubclassOf<AWeapon> StartingWeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	FName WeaponSocketName;

	UPROPERTY(BlueprintReadOnly)
	AWeapon* CurrentWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerCombatComponent* CombatComponent;

	//---------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Player stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	virtual void OnRep_Health();
	
	float Speed;
	
	float BaseWalkSpeed;
	float AimWalkSpeed;
	float RunSpeed;
	float ProneSpeed;

	UPROPERTY(BlueprintReadOnly)
	float AO_Yaw;
	
	UPROPERTY(BlueprintReadOnly)
	float AO_Pitch;

	float InterpAO_Yaw;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

public:
	FORCEINLINE UPlayerCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE AWeapon* GetWeapon() const { return CurrentWeapon; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace;}
	FVector GetHitTarget() const;
	ECombatStates GetCombatSate() const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation, Category = Animation)
	void MulticastPlayAnimMontage(class UAnimMontage* Montage);

};
