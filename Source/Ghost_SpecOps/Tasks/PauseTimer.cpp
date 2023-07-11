// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseTimer.h"

EStateTreeRunStatus UPauseTimer::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
	
	return Super::EnterState(Context, Transition);
}

void UPauseTimer::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
	Super::ExitState(Context, Transition);
}
