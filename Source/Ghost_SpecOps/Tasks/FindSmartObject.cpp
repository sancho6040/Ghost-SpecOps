// Fill out your copyright notice in the Description page of Project Settings.


#include "FindSmartObject.h"
#include "SmartObjectSubsystem.h"

EStateTreeRunStatus UFindSmartObject::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	// If a parent (or this state) is selected again consecutively, should we trigger a state change?
	// Should be false, as children are using their outputs
	bShouldStateChangeOnReselect = false;
	
	FBox SearchArea(Actor->GetActorLocation() - SearchRadius, Actor->GetActorLocation() + SearchRadius);

	// Build Request Filter
	FSmartObjectRequestFilter Filter;

	FGameplayTagQueryExpression Expression;
	Expression.AnyTagsMatch();
	Expression.AddTag(SmartObjectTag);
	
	Filter.ActivityRequirements.Build(Expression);
	
	FSmartObjectRequest Request(SearchArea, Filter);

	// Find Smart Object
	FSmartObjectRequestResult Result = GetWorld()->GetSubsystem<USmartObjectSubsystem>()->FindSmartObject(Request);

	if(Result.IsValid())
	{
		// Claim smart object
		SmartObjectClaimHandle = GetWorld()->GetSubsystem<USmartObjectSubsystem>()->Claim(
		Result.SmartObjectHandle, Result.SlotHandle);

		SmartObjectLocation = GetWorld()->GetSubsystem<USmartObjectSubsystem>()->GetSlotLocation(SmartObjectClaimHandle).GetValue();
	
		UE_LOG(LogTemp, Warning, TEXT("Claim success"));
		return EStateTreeRunStatus::Running;
	}
	UE_LOG(LogTemp, Error, TEXT("Claim failed."));
	return EStateTreeRunStatus::Failed;
}

void UFindSmartObject::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	//If, for some reason, the object went unused, release the claim now.
	if(SmartObjectClaimHandle.IsValid()
		&& GetWorld()->GetSubsystem<USmartObjectSubsystem>()->GetSlotState(SmartObjectClaimHandle.SlotHandle) == ESmartObjectSlotState::Claimed)
	{
		GetWorld()->GetSubsystem<USmartObjectSubsystem>()->Release(SmartObjectClaimHandle);
	}
	Super::ExitState(Context, Transition);
}
