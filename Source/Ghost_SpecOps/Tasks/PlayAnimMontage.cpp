// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayAnimMontage.h"

#include "GameFramework/Character.h"
#include "Ghost_SpecOps/BaseCharacter/BaseCharacter.h"

EStateTreeRunStatus UPlayAnimMontage::EnterState(FStateTreeExecutionContext& Context,
                                                 const FStateTreeTransitionResult& Transition)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
	if(Character)
	{
		Character->PlayAnimMontage(AnimMontage);
		Character->MulticastPlayAnimMontage(AnimMontage);
		Character->GetWorldTimerManager().SetTimer(AnimationTimer, this, &UPlayAnimMontage::EndState, AnimMontage->GetPlayLength());

		bIsRunning = true;
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UPlayAnimMontage::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return bIsRunning ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

void UPlayAnimMontage::EndState()
{
	bIsRunning = false;
}
