// Fill out your copyright notice in the Description page of Project Settings.


#include "RunEQSQuery.h"

#include "EnvironmentQuery/EnvQueryManager.h"

EStateTreeRunStatus URunEQSQuery::EnterState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition)
{
	FEnvQueryRequest Request(Query,Actor);
	
	Request.Execute(RunMode, this, &URunEQSQuery::FinishQuery);
	bIsFinished = false;
	bFailed = false;
	
	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus URunEQSQuery::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if(bFailed)
	{
		return EStateTreeRunStatus::Failed;
	}
	return Super::Tick(Context, DeltaTime);
}

void URunEQSQuery::FinishQuery(TSharedPtr<FEnvQueryResult> ResultPtr)
{
	FEnvQueryResult* Result = ResultPtr.Get();
	
	if(Result->IsSuccessful())
	{
		TargetLocation = Result->GetItemAsLocation(0);
		bIsFinished = true;
	}
	else
	{
		bFailed = true;
	}
}
