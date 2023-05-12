// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractComponent.h"
#include "SAttributesComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmCmp = CreateDefaultSubobject<USpringArmComponent>("SpringArmCmp");
	SpringArmCmp->SetupAttachment(RootComponent);
	SpringArmCmp->bUsePawnControlRotation = true;	//camera������Pawn���ϵ�playerController���������

	CameraCmp = CreateDefaultSubobject<UCameraComponent>("CameraCmp");
	CameraCmp->SetupAttachment(SpringArmCmp);

	InteractionComp = CreateDefaultSubobject<USInteractComponent>("InteractionComp");
	AttributeComp = CreateDefaultSubobject<USAttributesComponent>("AttributeComp");

	//��ת��ɫ������ٷ���
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	AttackAnimDelay = 0.17f;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}


// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();	//�ý�ɫ��ǰ������Ϊ�����Ƶķ���
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), value);
}

void ASCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	//X is Forward(Red) ; Y is Right(Green) ; Z is Up(Blue) ;
	//��characterת��camera��right���򣬶�����ת��character�����right����
	
	FVector rightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(rightVector, value);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("DashAttack", IE_Pressed, this, &ASCharacter::DashAttack);
	PlayerInputComponent->BindAction("ExplodeAttack", IE_Pressed, this, &ASCharacter::ExplodeAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

}



void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (ensureAlways(ClassToSpawn))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");	//�ӹ����л�ȡ��ӦSocket��λ��
		FVector TraceStartLocation = CameraCmp->GetComponentLocation();
		FVector TraceEndLocation = CameraCmp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);

		FCollisionObjectQueryParams ObjParams;	//������ײ����
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		FCollisionQueryParams Params;	//���ó���ʱ�������ֲ�����ײ
		Params.AddIgnoredActor(this);

		FActorSpawnParameters SpawnParameters;	//����SpawnActor()���������ɲ���
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;//����SpawnActor()���������ɲ���
		SpawnParameters.Instigator = this;

		FHitResult Hit;
		//GetWorld()->SweepSingleByObjectType(Hit, TraceStartLocation, TraceEndLocation, FQuat::Identity, ObjParams, Shape, Params);
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStartLocation, TraceEndLocation, FQuat::Identity, ObjParams,Shape, Params))
		{
			TraceEndLocation = Hit.ImpactPoint;
		}

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, TraceEndLocation);
		//FRotator SpawnRotation = FRotationMatrix::MakeFromX(TraceEndLocation - HandLocation).Rotator();  

		FTransform SpawnTM = FTransform(SpawnRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParameters);
		//DrawDebugLine(GetWorld(), HandLocation, Hit.ImpactPoint, FColor::Green, false, 2.f, 0, 2.f);
	}
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f && NewHealth > 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());	//������˸Ч��

	}
	if (NewHealth <= 0.0f && Delta <= 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());

		DisableInput(PC);
	}
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	//Assignment3-2-2:����ʹ��SpawnEmitterAttached����SpawnEmitterAtLocation��Answers:P46
	//����ʱ����ʹ��SpawnEmitterAtLocationʵ���ֲ����䵯��ʱ��������Ч��
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackHandVFX, GetMesh()->GetSocketLocation("Muzzle_01"), GetActorRotation());

	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::DashAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_DashAttack, this, &ASCharacter::DashAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::DashAttack_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::ExplodeAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_ExplodeAttack, this, &ASCharacter::ExplodeAttack_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::ExplodeAttack_TimeElapsed()
{
	SpawnProjectile(ExplodeProjectileClass);
}

//Exec����̨����
void ASCharacter::HealSelf(float Amount /* = 100.0*/)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}
