// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

class UWorld;
/**
 * 
 */
UCLASS(Blueprintable)
class LEARNINGPROJECTION_API USAction : public UObject
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	/*Tags added to owning actor when activated, removed when action stops */ 
	UPROPERTY(EditDefaultsOnly, category = "Tags")
	FGameplayTagContainer GrantsTags;

	/*Action can only start if owningActor has none of these Tags applied*/  
	UPROPERTY(EditDefaultsOnly, category = "Tags")
	FGameplayTagContainer BlockedTags;

	bool bIsRunning;

public:
	UFUNCTION(BlueprintCallable, category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Action")
	void StopAction(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, category = "Action")
	bool bAutoStart;

	/*Action nickname to start / stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, category = "Action")
	FName ActionName;

	UWorld* GetWorld() const override;
};
