// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "Ghost_SpecOps/BaseCharacter/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* HitActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& InHit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if(OwnerCharacter)
	{
		AController* OwnerController =  OwnerCharacter->Controller;
		if(OwnerController)
		{
			UGameplayStatics::ApplyDamage(HitActor, Damage, OwnerController, this, TSubclassOf<UDamageType>());

		}
	}

	
	Super::OnHit(HitComponent, HitActor, OtherComp, NormalImpulse, InHit);
}
