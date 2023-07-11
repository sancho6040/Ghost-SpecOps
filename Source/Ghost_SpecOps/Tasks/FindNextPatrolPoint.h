// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "FindNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UFindNextPatrolPoint : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
protected:

	/** Actor that the State Tree is attached to. 
	This task can only be executed on enemies */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Location of the next patrol point */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output")
	FVector TargetLocation;
	
};
