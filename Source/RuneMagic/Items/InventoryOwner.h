#pragma once
#include "InventoryType.h"

#include "InventoryOwner.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryOwner : public UInterface
{
	GENERATED_BODY()
};

class RUNEMAGIC_API IInventoryOwner
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=InventoryOwner)
	UUserWidget* CreateInventoryWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=InventoryOwner)
	EInventoryType GetType();
};
