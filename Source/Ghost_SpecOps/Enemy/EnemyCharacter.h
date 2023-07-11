// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter/BaseCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GHOST_SPECOPS_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	/** Starts the State Tree Logic */
	void StartStateTree() const;

	/** Taking damage */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void UpdateFocus();
	
	void UpdateAggroList(AActor* Actor, FAIStimulus Stimulus);

	/** Process visual stimuli */
	void ProcessVision(AActor* Actor, FAIStimulus Stimulus);

	/** Process incoming Perception Stimuli */
	UFUNCTION()
	void ProcessStimuli(AActor* Actor, FAIStimulus Stimulus);

	/** Send Alert event to State Tree */
	void Alert() const;

	/** Send Chase event to State Tree */
	void Chase();

	/** Turns off widget visibility */
	void HideWidget() const;

protected:
	
	/** Path for the enemy to follow (or guard location) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	class APatrolPath* PatrolPath;

	/** Current index in patrol path */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	int32 PatrolIndex;

	/** Enemy sees player or remembers seeing them */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	bool bCanSeePlayer;

	/** Used for flee state */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	bool bShouldRetreat;

	/** Used when seeing a corpse */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	bool bIsPermanentlyAlert;

	/** For investigation */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	FVector TargetLocation;

	/** For chasing / attacking */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	TArray<AActor*> AggroList;

	/** Direction of patrol. Only really useful when patrol doesn't cycle.
	True - Forwards, False - Backwards */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AILogic, Replicated)
	bool PatrolDirection;

	/** Death animation montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* DeathMontage;

	/** Alert Bark */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sound)
	class USoundCue* AlertSound;

	/** State Tree Logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStateTreeComponent* StateTreeComponent;

	/** AI Perception Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAIPerceptionComponent* PerceptionComponent;

	/** AI Perception Stimuli Source Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;

	/** Widget Component for Exclamation Mark */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEnemyCombatComponent* EnemyCombatComponent;

public:

	/** Is the enemy dead? */
	bool IsDead() const { return !bIsAlive; }

	/** Can the enemy see a player? */
	bool CanSeePlayer() const { return bCanSeePlayer; }

	/** Returns the patrol path */
	APatrolPath* GetPatrolPath() const { return PatrolPath; }

	/** Returns the patrol index */
	int32 GetPatrolIndex() const { return PatrolIndex; }

	/** Updates patrol index to the next point in sequence */
	void UpdatePatrolIndex();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation, Category = Animation)
	void MulticastChaseBark();

	AActor* GetFirstInAggro() const;

	bool HasAggro() const { return !AggroList.IsEmpty(); }

	UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }

	UStateTreeComponent* GetStateTreeComponent() const { return StateTreeComponent; }

	virtual void PlayFireMontage(bool bInAiming) const override;
	
};
