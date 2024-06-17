#pragma once
#include "InventoryType.h"

#include "InventoryOwner.generated.h"

class UInventoryOwnerWidget;

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
	UInventoryOwnerWidget* CreateInventoryWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=InventoryOwner)
	EInventoryType GetType();

	/** Find a suitable target inventory within an InventoryOwner, SourceInventory could be one within the owner, or an external inventory */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=InventoryOwner)
	UItemContainerComponent* GetTargetInventory(UItemContainerComponent* SourceInventory);
};
