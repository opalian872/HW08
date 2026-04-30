// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FWaveSpawnRow.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct FWaveDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Wave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ItemSpawnCount;
};
