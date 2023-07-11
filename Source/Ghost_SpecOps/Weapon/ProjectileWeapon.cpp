#include "ProjectileWeapon.h"

#include "Engine/StaticMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& InTarget)
{
	Super::Fire(InTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const UStaticMeshSocket* MuzzleFlasShocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlasShocket)
	{
		FTransform SocketTransform;
		MuzzleFlasShocket->GetSocketTransform(SocketTransform, GetWeaponMesh());
		
		FVector TargetDelta = InTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = TargetDelta.Rotation();
		
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World && Owner)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}
}


