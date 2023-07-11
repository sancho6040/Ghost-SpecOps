// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectRuntime.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "FindSmartObject.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UFindSmartObject : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:
	
	/** Actor that the State Tree is attached to.
	This task requires it to be a pawn for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Vector that defines the "search radius" for the actor. It's actually more of a box */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	FVector SearchRadius;

	/** Tag used to filter for the correct SmartObject */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	FGameplayTag SmartObjectTag;

	/** Claim handle for the Smart Object. This will be released upon state end if unused. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output")
	FSmartObjectClaimHandle SmartObjectClaimHandle;

	/** Location for the Smart Object */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output")
	FVector SmartObjectLocation;
	
};
