// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"),*GetNameSafe(this));
	
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

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
