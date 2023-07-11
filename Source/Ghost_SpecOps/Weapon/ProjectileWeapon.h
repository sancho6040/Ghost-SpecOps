// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

UCLASS()
class GHOST_SPECOPS_API AProjectileWeapon : public AWeapon
{
public:
	virtual void Fire(const FVector& InTarget) override;
	
private:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;
	
};
