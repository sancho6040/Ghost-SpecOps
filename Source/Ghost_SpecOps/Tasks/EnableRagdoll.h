// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "EnableRagdoll.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UEnableRagdoll : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:

	/** Actor that the State Tree is attached to.
	This task requires it to be a character for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;
	
};
