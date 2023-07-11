// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectRuntime.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "UseSmartObject.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UUseSmartObject : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:

	/** Actor that the State Tree is attached to.
	This task requires it to be a pawn for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Claim handle for the Smart Object passed in as input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	FSmartObjectClaimHandle SmartObjectClaimHandle;
	
};
