// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()

protected:

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* HitActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& InHit) override;

	
};
