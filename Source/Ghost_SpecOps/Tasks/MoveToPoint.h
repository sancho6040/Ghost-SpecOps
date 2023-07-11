// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "MoveToPoint.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API UMoveToPoint : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	EStateTreeRunStatus RequestMovement();

public:

	UFUNCTION()
	void FinishMovement(FAIRequestID RequestID, EPathFollowingResult::Type Result);

protected:

	/** Actor that the State Tree is attached to.
	This task requires it to be a pawn for it to work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** The target location to move to, passed in as input */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input")
	FVector TargetLocation;

	/** Should the move request be triggered on Enter State? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTriggerOnEnterState;

	/** Bind this boolean to a previous task output to ensure the move task happens after. 
	Only useable if bTriggerOnEnterState = false */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPreviousTaskFinished;

	/** Is the pawn performing a move action? */
	bool bIsMoving = false;

	/** Has the pawn requested a movement yet? */
	bool bMovementRequested = false;
	
};
