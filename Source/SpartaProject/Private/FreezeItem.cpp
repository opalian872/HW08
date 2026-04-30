// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeItem.h"
#include "SpartaCharacter.h"


AFreezeItem::AFreezeItem()
{
	ItemType = "Freeze";
	RequiredPressCount = 20;
}

void AFreezeItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (!Activator) return;

	ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator);
	if (!PlayerCharacter) return;

	PlayerCharacter->FreezePlayer(RequiredPressCount);

	DestroyItem();
}
