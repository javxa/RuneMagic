#include "InventoryBPFunctionLibrary.h"

void UInventoryBPFunctionLibrary::CreateItemStackFromClass(TSubclassOf<UItem> ItemType, int32 Count,
                                                           FItemStack& ItemStack)
{
	UItem* Item = NewObject<UItem>(GetTransientPackage(), ItemType);
	const FItemStack Result = {Item, Count};
	ItemStack = Result;
}

FString UInventoryBPFunctionLibrary::ItemStackDisplayName(const FItemStack ItemStack)
{
	if (!ItemStack.Count || !ItemStack.ItemType)
		return TEXT("None");
	return FString::Printf(TEXT("%s x%d"), *ItemStack.ItemType->Description.ToString(), ItemStack.Count);
}
