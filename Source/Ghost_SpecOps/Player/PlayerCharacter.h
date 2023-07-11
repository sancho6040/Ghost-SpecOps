#pragma once

#include "CoreMinimal.h"
#include "GhostPlayerController.h"
#include "Ghost_SpecOps/BaseCharacter/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"


//Forward declaration
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;
class UAIPerceptionStimuliSourceComponent;


UCLASS()
class GHOST_SPECOPS_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return CameraComponent; }

	void Die();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Die();

	UAIPerceptionStimuliSourceComponent* AIPerception;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY()
	AGhostPlayerController* PlayerController;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsInCover;

protected:

	FRotator StartingAimRotation;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	virtual void OnRep_Health() override;

	UFUNCTION()
	void UpdateHUDHealth();

	//------------------------------------------- Movement ------------------------------------------------------------
	
	void MoveForward(float InAxisValue);

	void MoveRight(float InAxisValue);

	void Turn(float InAxisValue);
	
	void TurnInPlace(float InAxisValue);
	
	//-------------------------------------------- Fire ---------------------------------------------------------------

	void OnFireButtonPressed();
	
	void OnFireButtonReleased();

	void CalculateAimOffset(float DeltaTime);

	void OnReloadButtonPressed();

	//---------------------------------------- Crouch & Prone ---------------------------------------------------------

	void OnCrouchButtonPressed();

	void OnProneButtonPressed();

	bool CanStandUp() const;

	void Prone();

	void StandUp();

	UFUNCTION(Server, Reliable)
	void Server_Prone();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Prone(const bool InStandUp);

	//---------------------------------------------- Aim --------------------------------------------------------------

	void OnAimButtonPressed();
	void OnAimButtonReleased();
	void SetAiming(bool bInIsAiming);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAiming(bool bInIsAiming);

	//---------------------------------------------- ADS --------------------------------------------------------------

	void OnADSButtonPressed();
	void OnADSButtonReleased();
	void SetAds(bool bInAds);

	//---------------------------------------------- Run --------------------------------------------------------------

	void OnRunButtonPressed();
	void OnRunButtonReleased();
	void SetRunning(bool bInIsRunning);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetRunning(bool bInIsRunning);

};
