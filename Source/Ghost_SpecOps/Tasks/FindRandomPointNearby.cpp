// Fill out your copyright notice in the Description page of Project Settings.


#include "FindRandomPointNearby.h"

#include "NavigationData.h"
#include "NavigationSystem.h"

EStateTreeRunStatus UFindRandomPointNearby::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if(!Actor)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	FNavLocation RandomPoint;
	UWorld* World = GetWorld();
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if(NavSys)
	{
		ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
		if(NavData)
		{
			bool bResult = NavSys->GetRandomReachablePointInRadius(
				Actor->GetActorLocation(),
				Radius,
				RandomPoint,
				NavData);

			RandomLocation = RandomPoint.Location;

			//Draw debug sphere
			//DrawDebugSphere(GetWorld(), RandomLocation, 50.f, 1, FColor::Red, false, 10.f);

			if(bResult)
			{
				// TODO: Experiment with this as Running.
				return EStateTreeRunStatus::Succeeded;
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}
