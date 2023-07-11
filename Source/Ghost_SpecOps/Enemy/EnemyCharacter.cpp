// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "AIController.h"
#include "../Tasks/PatrolPath.h"
#include "Components/SplineComponent.h"
#include "Components/StateTreeComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/StaticMeshSocket.h"
#include "Ghost_SpecOps/Civilian/CivilianCharacter.h"
#include "Ghost_SpecOps/Components/EnemyCombatComponent.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Sound/SoundCue.h"

AEnemyCharacter::AEnemyCharacter() :
	PatrolIndex(0),
	bCanSeePlayer(false),
	bShouldRetreat(false),
	bIsPermanentlyAlert(false),
	PatrolDirection(true)
{
	// Components
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("State Tree"));
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetVisibility(false);

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("CombatComponent"));
	EnemyCombatComponent->SetIsReplicated(true);

	// Delegate Binding
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::ProcessStimuli);

	// Actor Tags
	Tags.Add("Keycard");
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharacter, PatrolPath)
	DOREPLIFETIME(AEnemyCharacter, PatrolIndex)
	DOREPLIFETIME(AEnemyCharacter, bCanSeePlayer)
	DOREPLIFETIME(AEnemyCharacter, bShouldRetreat)
	DOREPLIFETIME(AEnemyCharacter, bIsPermanentlyAlert)
	DOREPLIFETIME(AEnemyCharacter, TargetLocation)
	DOREPLIFETIME(AEnemyCharacter, AggroList)
	DOREPLIFETIME(AEnemyCharacter, Health)
	DOREPLIFETIME(AEnemyCharacter, PatrolDirection)
}

void AEnemyCharacter::BeginPlay()
{
	if(HasAuthority())
	{
		FTimerHandle StartLogicHandle;
		GetWorldTimerManager().SetTimer(StartLogicHandle, this, &AEnemyCharacter::StartStateTree, 5.f, false);
	}
	Super::BeginPlay();
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	const APlayerCharacter* Player =  Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if(Player)
	{
		const FVector PlayerLocation = Player->GetFollowCamera()->GetComponentLocation();
		const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
	
		WidgetComponent->SetWorldRotation(Rotator);	
	}
	
	Super::Tick(DeltaSeconds);
}

void AEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(EnemyCombatComponent)
	{
		EnemyCombatComponent->EnemyCharacter = this;
	}
}

void AEnemyCharacter::StartStateTree() const
{
	StateTreeComponent->StartLogic();
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Client side stuff and error handling 	
	if(!HasAuthority())
	{
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	
	Health -= DamageAmount;

	// Rotate to face
	if(AggroList.IsEmpty())
	{
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			const FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*) (&DamageEvent);
			const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PointDamageEvent->ShotDirection);
			Controller->SetControlRotation(Rotator);
		}
		
	}
	
	// Death
	if (Health <= 0)
	{
		bIsAlive = false;
		const FGameplayTag DeathTag = DeathTag.RequestGameplayTag("Dead");
		const FStateTreeEvent DeathEvent(DeathTag);
		StateTreeComponent->SendStateTreeEvent(DeathEvent);
	}
	// Retreat
	else if(Health <= 20)
	{
		bShouldRetreat = true;
		const FGameplayTag RetreatTag = RetreatTag.RequestGameplayTag("Retreat");
		const FStateTreeEvent RetreatEvent(RetreatTag);
		StateTreeComponent->SendStateTreeEvent(RetreatEvent);
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemyCharacter::UpdateFocus()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if(!AggroList.IsEmpty())
	{
		if(AIController)
		{
			AIController->SetFocus(AggroList[0]);
		}
	}
	else
	{
		if(AIController)
		{
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
}

void AEnemyCharacter::UpdateAggroList(AActor* Actor, FAIStimulus Stimulus)
{
	// Player enters vision
	if(Stimulus.IsActive())
	{
		const bool bFirstAggro = AggroList.IsEmpty();
		AggroList.Add(Actor);
		if(bFirstAggro)
		{
			bIsAiming = true;
			Chase();
		}
	}
	// Player exits vision
	else if(!Stimulus.IsActive())
	{
		AggroList.Remove(Actor);

		// If vision lost on all players, investigate around area
		if(AggroList.IsEmpty())
		{
			bIsAiming = false;
			TargetLocation = Stimulus.StimulusLocation;
			Alert();
		}
	}
}

void AEnemyCharacter::ProcessVision(AActor* Actor, FAIStimulus Stimulus)
{
	// Enemy / Civilian sourced stimuli
	if(Stimulus.IsActive() && !bIsPermanentlyAlert && (Actor->IsA<AEnemyCharacter>() || Actor->IsA<ACivilianCharacter>()))
	{
		// Permanently alert on seeing corpse
		const ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
		if(!Character->bIsAlive)
		{
			bIsPermanentlyAlert = true;
			TargetLocation = Stimulus.StimulusLocation;
			if(!CanSeePlayer())
			{
				Alert();	
			}
		}
	}
	// Player-sourced stimuli
	else if(Actor->IsA<APlayerCharacter>())
	{
		// Aggro
		UpdateAggroList(Actor, Stimulus);

		// Focus
		UpdateFocus();
		
		// Update CanSeePlayer variable
		bCanSeePlayer = !AggroList.IsEmpty();
	}
}

void AEnemyCharacter::ProcessStimuli(AActor* Actor, FAIStimulus Stimulus)
{
	// Error handling
	if(!bIsAlive)
	{
		return;
	}
	
	// Handle Vision
	if(Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		ProcessVision(Actor, Stimulus);
	}
	// Handle Hearing
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// Investigate
		if(Stimulus.IsActive() && !bCanSeePlayer)
		{
			TargetLocation = Stimulus.StimulusLocation;
			Alert();
		}
	}
}

void AEnemyCharacter::Alert() const
{
	const FGameplayTag Tag = Tag.RequestGameplayTag("Alert");
	StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(Tag));
}

void AEnemyCharacter::Chase()
{
	// Bark for Chase
	if(HasAuthority())
	{
		MulticastChaseBark();	
	}
	
	const FGameplayTag Tag = Tag.RequestGameplayTag("Chase");
	StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(Tag));	
}

void AEnemyCharacter::HideWidget() const
{
	WidgetComponent->SetVisibility(false);
}

void AEnemyCharacter::PlayFireMontage(bool bAiming) const
{
	if (!EnemyCombatComponent || !CurrentWeapon || !CurrentWeapon->GetWeaponMesh()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	FTransform SocketTransform{};

	const UStaticMeshSocket* BarrelSocket = CurrentWeapon->GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (AnimInstance && FireWeaponMontage && CurrentWeapon->GetFireSound() && CurrentWeapon->GetMuzzleFlash() && BarrelSocket)
	{
		BarrelSocket->GetSocketTransform(SocketTransform, CurrentWeapon->GetWeaponMesh());
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			CurrentWeapon->GetMuzzleFlash(),
			SocketTransform
			);
		
		UGameplayStatics::PlaySoundAtLocation(this, CurrentWeapon->GetFireSound(), GetActorLocation(), GetActorRotation(), 0.5f);
		
		AnimInstance->Montage_Play(FireWeaponMontage);
		const FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEnemyCharacter::UpdatePatrolIndex()
{
	if(const USplineComponent* Path = PatrolPath->GetPath())
	{
		// If it's a single point...
		if(Path->GetNumberOfSplinePoints() == 1)
		{
			return;
		}
		
		// If it's a closed loop...
		if(Path->IsClosedLoop())
		{
			if(PatrolIndex + 1 >= Path->GetNumberOfSplinePoints())
			{
				PatrolIndex = 0;
			}
			else
			{
				++PatrolIndex;
			}
		}
		// If it's not...
		else
		{
			if(PatrolDirection)
			{
				if(PatrolIndex + 1 >= Path->GetNumberOfSplinePoints())
				{
					PatrolDirection = false;
					--PatrolIndex;
				}
				else
				{
					++PatrolIndex;
				}
			}
			else
			{
				if(PatrolIndex - 1 < 0)
				{
					PatrolDirection = true;
					++PatrolIndex;
				}
				else
				{
					--PatrolIndex;
				}
			}
		}
		
	}
}

AActor* AEnemyCharacter::GetFirstInAggro() const
{
	if(!AggroList.IsEmpty())
	{
		return AggroList[0];
	}
	return nullptr;
}

void AEnemyCharacter::MulticastChaseBark_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, AlertSound, GetActorLocation(), GetActorRotation());
	WidgetComponent->SetVisibility(true);

	FTimerHandle TurnOffWidgetHandle;
	GetWorldTimerManager().SetTimer(TurnOffWidgetHandle, this, &AEnemyCharacter::HideWidget, 2.f, false);
}

bool AEnemyCharacter::MulticastChaseBark_Validate()
{
	return true;
}
