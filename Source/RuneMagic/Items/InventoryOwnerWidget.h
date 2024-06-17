

#pragma once

#include "CoreMinimal.h"
#include "ItemContainerComponent.h"
#include "InventoryWidgetManager.h"
#include "ItemClickType.h"
#include "ItemContainerWidget.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "InventoryOwnerWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RUNEMAGIC_API UInventoryOwnerWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UInventoryWidgetManager> InventoryWidgetManager;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void BindItemContainer(UItemContainerWidget* ItemContainer, UItemContainerComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void ItemClicked(UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType);
};
