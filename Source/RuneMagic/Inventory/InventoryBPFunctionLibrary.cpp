#include "InventoryBPFunctionLibrary.h"
#include "RuneMagic/Items/ItemStack.h"

void UInventoryBPFunctionLibrary::CreateItemStackFromClass(TSubclassOf<UItem> ItemClass, int32 Count,
                                                           FItemStack& ItemStack)
{
	UItem* Item = NewObject<UItem>(GetTransientPackage(), ItemClass);
	const FItemStack Result = {Item, Count};
	ItemStack = Result;
}

FString UInventoryBPFunctionLibrary::ItemStackDisplayName(const FItemStack ItemStack)
{
	if (!ItemStack.Count || !ItemStack.ItemType)
		return TEXT("None");
	return FString::Printf(TEXT("%s x%d"), *ItemStack.ItemType->Description.ToString(), ItemStack.Count);
}

int32 UInventoryBPFunctionLibrary::RoomInStack(UItemContainerComponent* Inventory, const FItemStack ItemStack)
{
	return Inventory->GetStackSize(ItemStack.ItemType) - ItemStack.Count;
}
