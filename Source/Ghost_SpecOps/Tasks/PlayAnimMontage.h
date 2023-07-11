// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "PlayAnimMontage.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UPlayAnimMontage : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	void EndState();

protected:

	/** Actor that the State Tree is attached to.
	This task requires it to be a character for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** Animation Montage passed in as Input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	class UAnimMontage* AnimMontage;

	/** Timer for the animation length */
	FTimerHandle AnimationTimer;

	/** Task Running? */
	bool bIsRunning;
	
};
