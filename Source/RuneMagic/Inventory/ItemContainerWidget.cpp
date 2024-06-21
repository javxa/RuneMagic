#include "ItemContainerWidget.h"

#include "InventoryOwnerWidget.h"

void UItemContainerWidget::BindInventory(UItemContainerComponent* Inventory, UInventoryOwnerWidget* OwnerWidget)
{
	this->InventoryComponent = Inventory;
	this->InventoryOwnerWidget = OwnerWidget;
	
	Inventory->OnInventoryUpdated.AddDynamic(this, &UItemContainerWidget::UpdateInventoryItems);
	this->UpdateInventoryItems();
}

void UItemContainerWidget::ItemClicked(const int32 ItemIndex, const EItemClickType ClickType)
{
	InventoryOwnerWidget->ItemClicked(InventoryComponent, ItemIndex, ClickType);
}
