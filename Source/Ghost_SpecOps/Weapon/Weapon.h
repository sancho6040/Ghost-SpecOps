#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ghost_SpecOps/Player/GhostPlayerController.h"
#include "Ghost_SpecOps/Types/WeaponTypes.h"
#include "Weapon.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UAnimationAsset;
class UTexture2D;
class USoundCue;
class UParticleSystem;
class APlayerCharacter;
class AGhostPlayerController;

UCLASS()
class GHOST_SPECOPS_API AWeapon : public AActor
{
private:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon properties")
	UAnimationAsset* FireAnimation;

	// Zoom variables

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendAmmo();
	
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	AGhostPlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	EWeaponTypes WeaponType;

protected:

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon properties")
	USkeletalMeshComponent* WeaponSkeletalMesh; // if animated

	UPROPERTY(EditDefaultsOnly)
	float ShootDistance;

	UPROPERTY(EditDefaultsOnly)
	float ShootDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;
	
	virtual void BeginPlay() override;

public:	
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	virtual void Fire(const FVector& InTarget);
	void AddAmmo(int32 AmmoToAdd);
	
	// Weapon crosshair testures
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;


	//Automatic fire variables
	UPROPERTY(EditAnywhere, Category = combat)
	float FireDelay = 0.3;
	UPROPERTY(EditAnywhere, Category = combat)
	bool bIsAutomatic = true;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UStaticMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	FORCEINLINE USoundCue* GetFireSound() const {return FireSound;}
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const {return MuzzleFlash;}
	FORCEINLINE float GetZoomedFOV() const {return  ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return  ZoomedFOV;}
	FORCEINLINE EWeaponTypes GetWeaponType() const {return WeaponType;}
	FORCEINLINE int32 GetAmmo() const {return Ammo;}
	FORCEINLINE int32 GetMagCapacity() const {return MagCapacity;}

	bool IsEmpty();
	
};
