// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCombatComponent.h"

#include "Ghost_SpecOps/Enemy/EnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyCombatComponent::UEnemyCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UEnemyCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(EnemyCharacter)
	{
		FHitResult HitResult;
		TraceAroundTarget(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
}

void UEnemyCombatComponent::Fire()
{
	if(bCanFire)
	{
		bCanFire = false;
		MulticastFire(HitTarget);
		StartFireTimer();
	}
}

void UEnemyCombatComponent::OrderToFire(bool bInState)
{
	bOrderedToFire = bInState;
	if(bOrderedToFire)
	{
		Fire();
	}
}

void UEnemyCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& InTraceHitTarget)
{
	if(!EnemyCharacter->GetWeapon())
	{
		return;
	}
	if(EnemyCharacter)
	{
		EnemyCharacter->PlayFireMontage(EnemyCharacter->bIsAiming);
		EnemyCharacter->GetWeapon()->Fire(InTraceHitTarget);
	}
}

void UEnemyCombatComponent::StartFireTimer()
{
	if(EnemyCharacter && EnemyCharacter->GetWeapon())
	{
		EnemyCharacter->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UEnemyCombatComponent::FinishFireTimer,
			EnemyCharacter->GetWeapon()->FireDelay);
	}
}

void UEnemyCombatComponent::FinishFireTimer()
{
	bCanFire = true;
	if(bOrderedToFire && EnemyCharacter->GetWeapon()->bIsAutomatic)
	{
		Fire();
	}
}

void UEnemyCombatComponent::TraceAroundTarget(FHitResult& TraceHitResult)
{
	FVector Start = EnemyCharacter->GetWeapon()->GetWeaponMesh()->GetSocketLocation("Muzzle Flash");
	Start += EnemyCharacter->GetActorForwardVector() * 50.0f;

	AActor* TargetActor = EnemyCharacter->GetFirstInAggro();

	if(TargetActor)
	{
		FVector TargetDirection = TargetActor->GetActorLocation() - EnemyCharacter->GetActorLocation();
		TargetDirection.Normalize();

		FVector End = Start + TargetDirection * TRACE_LENGTH;

		FRotator RandomRotator;
		RandomRotator.Pitch = FMath::RandRange(-10.f, 10.f);
		RandomRotator.Yaw = FMath::RandRange(-10.f, 10.f);

		End = RandomRotator.RotateVector(End);

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
		
	}
	
}
