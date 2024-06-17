

#pragma once

#include "CoreMinimal.h"
#include "ItemContainerComponent.h"
#include "InventoryWidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainerWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RUNEMAGIC_API UItemContainerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UInventoryOwnerWidget> InventoryOwnerWidget;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UItemContainerComponent> InventoryComponent;

public:
	UFUNCTION()
	void BindInventory(UItemContainerComponent* Inventory, UInventoryOwnerWidget* OwnerWidget);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateInventoryItems();

	UFUNCTION(BlueprintCallable)
	void ItemClicked(int32 ItemIndex, EItemClickType ClickType);
};
