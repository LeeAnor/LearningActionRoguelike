// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SAttributesComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SGamePlayFunctionLibrary.h"
#include "SActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor!=GetInstigator())
	{
		//FName Muzzle = "Muzzle_01";

		/* static ���ڵ��ô˺�����һֱ�������ڴ���,������Ϊ������ֹ����ʧ,����һ�ε��ô˺�����ʱ�򲻱��ٴ����� */
		//static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Status.Parrying");

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = - MovementComp->Velocity;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		/*USAttributesComponent* AttributeComp = USAttributesComponent::GetAttributes(OtherActor);
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(GetInstigator(), DamageAmount);
		}
		Explode();*/

		if (USGamePlayFunctionLibrary::ApplyDirectionDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();
		}
	}

}