#include "BaseCharacterAnimInstance.h"

#include "BaseCharacter.h"
#include "KismetAnimationLibrary.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseCharacterAinmInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());

	bIsPlayer = false;
	if(Cast<APlayerCharacter>(TryGetPawnOwner()))
	{
		bIsPlayer = true;
	}
}

void UBaseCharacterAinmInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(!BaseCharacter)
	{
		BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	}
	if(!BaseCharacter)
	{
		return;
	}

	FVector Velocity = BaseCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	
	const FRotator ActorRotation = BaseCharacter->GetActorRotation();
	CharacterDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);

	//Setting the sate control variables
	EquippedWeapon = BaseCharacter->GetWeapon();
	bIsCrouched = BaseCharacter->bIsCrouched;
	bIsAiming = BaseCharacter->bIsAiming;
	bIsProne = BaseCharacter->bIsProne;
	bIsStanding = BaseCharacter->bIsStanding;
	bIsRunning = BaseCharacter->bIsRunning;
	bIsAlive = BaseCharacter->bIsAlive;
	TurningInPlace = BaseCharacter->GetTurningInPlace();

	AO_Yaw = BaseCharacter->GetAO_Yaw();
	AO_Pitch = BaseCharacter->GetAO_Pitch();

	//FABRIK IF
	if (EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BaseCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BaseCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		//Weapon hand align rotation to hit target
		if(BaseCharacter->IsLocallyControlled())
		{
			bIsLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BaseCharacter->GetHitTarget()));
			DistanceToTarget = (RightHandTransform.GetLocation() - BaseCharacter->GetHitTarget()).Size();


			//...for debug
			// FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), RTS_World);
			// FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
			// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BaseCharacter->GetHitTarget(), FColor::Orange);
			
		}
	}

	bUseFabrik = BaseCharacter->GetCombatSate() != ECombatStates::ECS_Reloading;
	bUseAimOffsets = BaseCharacter->GetCombatSate() != ECombatStates::ECS_Reloading;
	bTransformRightHand = BaseCharacter->GetCombatSate() != ECombatStates::ECS_Reloading;

}
