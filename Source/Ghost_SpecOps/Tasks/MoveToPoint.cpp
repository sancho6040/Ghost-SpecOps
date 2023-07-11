// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPoint.h"

#include "AIController.h"

EStateTreeRunStatus UMoveToPoint::EnterState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition)
{
	if(!bTriggerOnEnterState)
	{
		bIsMoving = false;
		return EStateTreeRunStatus::Running;
	}
	return RequestMovement();
}

void UMoveToPoint::FinishMovement(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bIsMoving = false;
}

EStateTreeRunStatus UMoveToPoint::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if(!bTriggerOnEnterState)
	{
		if(bIsPreviousTaskFinished && !bIsMoving && !bMovementRequested)
		{
			return RequestMovement();
		}
	}
	return bIsMoving ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

void UMoveToPoint::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if(Pawn)
	{
		AAIController* Controller = Cast<AAIController>(Pawn->GetController());
		if(Controller)
		{
			Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UMoveToPoint::FinishMovement);
			Controller->StopMovement();
		}
	}
	
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus UMoveToPoint::RequestMovement()
{
	APawn* Pawn = Cast<APawn>(Actor);
	if(Pawn)
	{
		AAIController* Controller = Cast<AAIController>(Pawn->GetController());
		if(Controller)
		{
			const FPathFollowingRequestResult RequestResult = Controller->MoveTo(TargetLocation);
			bIsMoving = true;
			bMovementRequested = true;

			Controller->ReceiveMoveCompleted.AddUniqueDynamic(this, &UMoveToPoint::FinishMovement);

			return EStateTreeRunStatus::Running;
		}
	}
	return EStateTreeRunStatus::Failed;
}


