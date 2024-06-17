#include "InventoryOwnerWidget.h"

void UInventoryOwnerWidget::BindItemContainer(UItemContainerWidget* ItemContainer, UItemContainerComponent* Inventory)
{
	ItemContainer->BindInventory(Inventory, this);
}

void UInventoryOwnerWidget::ItemClicked(UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType)
{
	InventoryWidgetManager->ItemClicked(this, Inventory, ItemIndex, ClickType);
}
