// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "FireWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UFireWeapon : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void ChangeFire();

protected:

	/** Actor that the State Tree is attached to.
	This task requires it to be an enemy character for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Timer for firing */
	FTimerHandle FireTimer;

	/** Current fire state */
	bool bIsFiring = false;
	
};
