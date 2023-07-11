#include "Weapon.h"
#include "Ghost_SpecOps/Ghost_SpecOps.h"
#include "Ghost_SpecOps/Components/PlayerCombatComponent.h"
#include "Ghost_SpecOps/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

	ShootDistance = 10000.f;
	ShootDamage = 20.f;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	SetHUDAmmo();
}

void AWeapon::SetHUDAmmo()
{
	PlayerCharacter = PlayerCharacter == nullptr ?  Cast<APlayerCharacter>(GetOwner()) : PlayerCharacter;
	if(PlayerCharacter)
	{
		PlayerController = PlayerController == nullptr ? Cast<AGhostPlayerController>(PlayerCharacter->Controller) : PlayerController;
		if(PlayerController)
		{
			PlayerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::Fire(const FVector& InTarget)
{
	if(FireAnimation && ImpactParticles)
	{
		WeaponSkeletalMesh->PlayAnimation(FireAnimation, false);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, InTarget);// not working
	}

	SpendAmmo();
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

void AWeapon::SpendAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity );
	SetHUDAmmo();
}

void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}



