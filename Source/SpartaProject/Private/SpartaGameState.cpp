#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "CoinItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	WaveSpawnedCoinCount = 0;
	WaveCollectedCoinCount = 0;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
	WaveDataTable = nullptr;
}
void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}


void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance) SpartaGameInstance->AddToScore(Amount);
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Score: %d"), Score);
}



void ASpartaGameState::StartLevel()
{
	ClearCurrentWaveActors();
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance) CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
	}
	CurrentWaveIndex = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveSpawnedCoinCount = 0;
	WaveCollectedCoinCount = 0;

	StartWave();

	//TArray<AActor*> FoundVolumes;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	//const int32 ItemToSpawn = 40;
	//for (int32 i = 0; i < ItemToSpawn; i++)
	//{
	//	if (FoundVolumes.Num() > 0)
	//	{
	//		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	//		if (SpawnVolume)
	//		{
	//			AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
	//			if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
	//			{
	//				SpawnedCoinCount++;
	//			}
	//		}
	//	}
	//}

	//GetWorldTimerManager().SetTimer(
	//	WaveTimerHandle,
	//	this,
	//	&ASpartaGameState::OnLevelTimeUp,
	//	LevelDuration,
	//	false
	//);
	//UE_LOG(LogTemp, Warning, TEXT("Level %d Start! Spawned %d coin"), CurrentLevelIndex + 1, SpawnedCoinCount);
}



void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	WaveCollectedCoinCount++; 

	//UE_LOG(LogTemp, Warning, TEXT("Coin Colledted: %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (WaveSpawnedCoinCount > 0 && WaveCollectedCoinCount >= WaveSpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}


void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{			
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}



void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Game Over!!"));
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimerValue"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreValue"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}
				if (UTextBlock* CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CoinValue"))))
				{
					CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin: %d / %d"), CollectedCoinCount, SpawnedCoinCount)));					
				}
				if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelValue"))))
				{
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex+1)));
				}
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveValue"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex + 1)));
				}
			}
		}
	}
}

void ASpartaGameState::StartWave()
{
	if (CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
		return;
	}

	
	if (!WaveDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveDataTable is nullptr"));
		return;
	}

	const FName RowName = FName(*FString::Printf(
		TEXT("Level_%d_Wave_%d"),
		CurrentLevelIndex,
		CurrentWaveIndex
	));

	const FWaveDataRow* WaveData = WaveDataTable->FindRow<FWaveDataRow>(
		RowName,
		TEXT("Find Wave Data")
	);

	if (!WaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No WaveData found: %s"), *RowName.ToString());
		return;
	}

	WaveSpawnedCoinCount = 0;
	WaveCollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASpawnVolume::StaticClass(),
		FoundVolumes
	);

	if (FoundVolumes.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpawnVolume found"));
		return;
	}

	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	if (!SpawnVolume)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnVolume cast failed"));
		EndWave();
		return;
	}

	
	for (int32 i = 0; i < WaveData->ItemSpawnCount; i++)
	{
		AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

		if (SpawnedActor)
		{
			CurrentWaveActors.Add(SpawnedActor);
			if (SpawnedActor->IsA(ACoinItem::StaticClass()))
			{
				SpawnedCoinCount++;
				WaveSpawnedCoinCount++;
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::EndWave,
		WaveData->WaveDuration,
		false
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Start Wave: %s / SpawnRate: %.1f / Coins: %d / Duration: %.1f"),
		*RowName.ToString(),
		WaveData->ItemSpawnCount,
		SpawnedCoinCount,
		WaveData->WaveDuration
	);
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	ClearCurrentWaveActors();

	CurrentWaveIndex++;

	StartWave();
}

void ASpartaGameState::ClearCurrentWaveActors()
{
	for (AActor* Actor : CurrentWaveActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	CurrentWaveActors.Empty();
}
