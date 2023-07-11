// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GHOST_SPECOPS_API UEnemyCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyCombatComponent();

	friend class AEnemyCharacter;
	friend class ABaseCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OrderToFire(bool bInState);

protected:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& InTraceHitTarget);

	void TraceAroundTarget(FHitResult& TraceHitResult);

private:	
	
	class AEnemyCharacter* EnemyCharacter;

	bool bOrderedToFire;

	FVector HitTarget;

	void Fire();

	FTimerHandle FireTimer;

	bool bCanFire = true;
	void StartFireTimer();
	void FinishFireTimer();

		
};
