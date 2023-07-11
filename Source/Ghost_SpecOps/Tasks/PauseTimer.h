// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "PauseTimer.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UPauseTimer : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:

	/** Timer Handle passed in via input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	FTimerHandle TimerHandle;
};
