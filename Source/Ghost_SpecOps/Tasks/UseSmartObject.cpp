// Fill out your copyright notice in the Description page of Project Settings.


#include "UseSmartObject.h"

#include "FindSmartObject.h"
#include "SmartObjectSubsystem.h"
#include "SmartObjectDefinition.h"

EStateTreeRunStatus UUseSmartObject::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition)
{
	GetWorld()->GetSubsystem<USmartObjectSubsystem>()->Use<USmartObjectBehaviorDefinition>(SmartObjectClaimHandle);

	// Smart Object logic goes here...
	// Unfortunately, USmartObjectGameplayBehaviorDefinition could not be found. This task has been rewritten using blueprints instead.
	
	return Super::EnterState(Context, Transition);
}
