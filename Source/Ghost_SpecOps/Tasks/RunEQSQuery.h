// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "RunEQSQuery.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API URunEQSQuery : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	
	void FinishQuery(TSharedPtr<FEnvQueryResult> Result);

protected:

	/** Actor that the State Tree is attached to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Context")
	AActor* Actor;

	/** The Query to Run */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEnvQuery* Query;

	/** The Run Mode for the Query */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

	/** Output Location  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FVector TargetLocation;

	/** Output boolean. Useful for later tasks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bIsFinished;

	/** To end state early */
	bool bFailed;
};
