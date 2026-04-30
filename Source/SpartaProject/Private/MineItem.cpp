// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

AMineItem::AMineItem()
{	
	ExplosionDelay = 5.f;
	ExplosionRadius = 300.f;
	ExplosionDamage = 30;
	ItemType = "Mine";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(300.f);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);

	MineCountWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MineCountWidget"));
	MineCountWidget->SetupAttachment(StaticMesh);
	MineCountWidget->SetWidgetSpace(EWidgetSpace::Screen);
	MineCountWidget->SetVisibility(false);
	CurrentMineCount = 5;
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	UParticleSystemComponent* Particle = nullptr;
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}
	if (Particle)
	{
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			ExplosionDelay-0.5f,
			false
		);
	}
	
	CurrentMineCount = FMath::CeilToInt(ExplosionDelay);

	if (MineCountWidget)
	{
		MineCountWidget->SetVisibility(true);
	}

	GetWorld()->GetTimerManager().SetTimer(
		MineCountTimerHandle,
		this,
		&AMineItem::UpdateMineCount,
		1.0f,
		true
	);



	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false
	);
	bHasExploded = true;

}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Player damaged %d by MineItem"), ExplosionDamage));
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(MineCountTimerHandle);

	DestroyItem();

	if (Particle)
	{
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.f,
			false
		);
	}
}

void AMineItem::UpdateMineCount()
{
	CurrentMineCount--;

	if (CurrentMineCount <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(MineCountTimerHandle);
		return;
	}

	if (!MineCountWidget) return;

	UUserWidget* UserWidget = MineCountWidget->GetUserWidgetObject();
	if (!UserWidget) return;

	UTextBlock* CountText = Cast<UTextBlock>(
		UserWidget->GetWidgetFromName(TEXT("MineCount"))
	);

	if (!CountText) return;

	CountText->SetText(
		FText::FromString(FString::Printf(TEXT("%d..."), CurrentMineCount))
	);
}
