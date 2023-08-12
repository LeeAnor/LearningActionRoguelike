// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	/*
	* ֻ�е�CanStart()����trueʱ���Ż�ִ��StartAction
	* �����һ������StartActionʹbIsRunning=true�󣬺�����CanStart()����false����������StartAction����ִ��
	*/
	if (bIsRunning)
	{
		return false;
	}

	USActionComponent* Comp = GetOwningComponent();

	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	return true;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"),*GetNameSafe(this));
	
	USActionComponent* Comp = GetOwningComponent();

	/*
	* AddTagֻ����ӵ���Tag��TagContainer��
	* AppendTags������������TagContainer����һ��TagContainer��
	*/
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	/* 
	* �����������������StartAction��ʹ�ö�ʱ���ӳ�0.17s��ʱ�����ڲ��ŻӶ��ֱ۶�����0.17s֮��������Projectile������StopAction
	* ��ÿһ����ʱ�����첽ִ�У�
	* �ڵ�һ��StartAction���һ��StopAction�ļ��ʱ���У�bIsRunning��ֵһֱΪ�棻
	* ����֮����õ����ɸ�StartAction�������첽ִ�п��ܻ���֣���һ��StopActionʹbIsRunning=false,�������Ż᲻ִ�еڶ���StartactionʹbIsRunning=true
	* ȴ����ִ�еڶ���StopAction����ʱ��Ϊ�첽������ִ������StopAction
	* �ڶ��ε�StopAction��bIsRunning=false�����¶���ensureAlways(bIsRunning)����
	* 
	* ��CanStart()�м��� if(bIsRunning) return false; ���������������⣬��֤bIsRunningʼ��Ϊtrue
	*/
	ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();

	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

UWorld* USAction::GetWorld() const
{
	/*
		T* NewObject(UObject* Outer, const UClass* Class, ......)
		��SActionCompoent->AddAction�е�NewObject<USAction>(this, ActionClass)�������this���Ǵ�ʱ��Outer
		Outer is set when creating action via NewObject<1>
	*/
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());

	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	/*��SActionCompoent->AddAction�е�NewObject<USAction>(this, ActionClass)��֤��Outerһ����Ϊ��*/
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::IsRunning() const
{
	return bIsRunning;
}