#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI) // 여기는 리플렉션을 위한 API. 작성 필요 없음
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};



class SPARTAPROJECT_API IItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 지뢰, 힐링 , 코인
	// 힐링/코인은 overlap 즉시 발동
	// 지뢰는? 범위 내에 overlap 시 5초 뒤 폭발. 5초 뒤에도 overlap 이면 데미지
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0; //AActor인 이유 - 확장성을 위해. 
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0; //=0 넣으면 override 강제. 인터페이스엔 당연히 들어가야 한다
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0; //FName - 타입명 이런거 알아내고 싶을 때 빠르고 가볍게 쓸 수 있는 자료형.



};
