// Fill out your copyright notice in the Description page of Project Settings.


#include "SBaseProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASBaseProjectile::ASBaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("sphereComp");
	SphereComp->SetCollisionProfileName("Projectile");	//ͨ���Զ�������������ײ����(��ײԤ��)������Ŀ����->��ײ->Preset���Զ���
	SphereComp->OnComponentHit.AddDynamic(this, &ASBaseProjectile::OnActorHit);

	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(RootComponent);
}


void ASBaseProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ASBaseProjectile::Explode_Implementation()
{
	if (ensure(IsValid(this)))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation(), GetActorRotation());	//������Ч

		AudioComp->Stop();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactCue, GetActorLocation(), GetActorRotation());	//������Ч

		Destroy();
	}
}

void ASBaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASBaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}


