// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "FreezeItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API AFreezeItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AFreezeItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ice")
	int32 RequiredPressCount;

	virtual void ActivateItem(AActor* Activator) override;
};
