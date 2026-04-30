#include "Item.h"

DEFINE_LOG_CATEGORY(LogSparta);

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	
	UE_LOG(LogSparta, Error, TEXT("%s Constructor"), *GetName());
	RotationSpeed = 100.0f;
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();	
	UE_LOG(LogSparta, Error, TEXT("%s PostInitializeComponents"), *GetName());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("My Item Appears!")); //Warning 부분이 로그 색깔. Display - Warning - Error 순
	UE_LOG(LogSparta, Error, TEXT("My Sparta!"));
	UE_LOG(LogSparta, Error, TEXT("%s BeginPlay"), *GetName());
	OnItemPickedUp();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogSparta, Error, TEXT("%s Tick"), *GetName());
	if (!FMath::IsNearlyZero(RotationSpeed)) 
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed*DeltaTime, 0.0f));
	}
}
void AItem::Destroyed()
{
	Super::Destroyed();
	UE_LOG(LogSparta, Error, TEXT("%s Destroyed"), *GetName());
}
void AItem::EndPlay(const EEndPlayReason::Type EndplayReason)
{
	Super::EndPlay(EndplayReason);
	UE_LOG(LogSparta, Error, TEXT("%s EndPlay"), *GetName());
}


void AItem::ResetActorPosition()
{
	SetActorLocation(FVector::ZeroVector);
}
float AItem::GetRotationSpeed() const
{
	return RotationSpeed;
}
