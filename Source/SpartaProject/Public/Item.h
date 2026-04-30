// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "Item.generated.h" // 무조건 마지막

DECLARE_LOG_CATEGORY_EXTERN(LogSparta, Warning, All); //카테고리 이름, Warning 이상만 출력, 필요하면 All 로그 활성화 가능

UCLASS() //UCLASS(Blueprintable, BlueprintType) 이게 기본 생성자. 사실 없어도 똑같음. 블루프린트에서 상속 가능하다 Blueprintable.
//블루프린트에서 이 클래스를 참조, 변수로 선언 가능하다 -> BlueprintType
//UCLASS(BlueprintType) 하면 참조, 변수선언은 가능한데 상속은 불가능한 이상한 게 된다.
class SPARTAPROJECT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
	float RotationSpeed;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndplayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Item|Actions")
	void ResetActorPosition();

	UFUNCTION(BlueprintPure, Category = "Item|Properties")
	float GetRotationSpeed() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnItemPickedUp();
};
