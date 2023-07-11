// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "FindRandomPointNearby.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UFindRandomPointNearby : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;


protected:

	/** Actor that the State Tree is attached to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Search radius passed in via input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	float Radius;

	/** Location chosen, as output */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Output")
	FVector RandomLocation;
	
};
