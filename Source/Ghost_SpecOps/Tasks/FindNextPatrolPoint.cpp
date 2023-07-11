// Fill out your copyright notice in the Description page of Project Settings.


#include "FindNextPatrolPoint.h"

#include "../Enemy/EnemyCharacter.h"
#include "PatrolPath.h"
#include "Components/SplineComponent.h"

EStateTreeRunStatus UFindNextPatrolPoint::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Actor);
	if(EnemyCharacter)
	{
		APatrolPath* PatrolPath = EnemyCharacter->GetPatrolPath();
		if(PatrolPath)
		{
			TargetLocation = PatrolPath->GetPath()->GetLocationAtSplinePoint(EnemyCharacter->GetPatrolIndex(), ESplineCoordinateSpace::World);
			EnemyCharacter->UpdatePatrolIndex();
			return EStateTreeRunStatus::Running;
		}
	}
	return EStateTreeRunStatus::Failed;
}
