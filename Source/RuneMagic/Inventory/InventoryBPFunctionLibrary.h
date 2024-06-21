

#pragma once

#include "CoreMinimal.h"
#include "RuneMagic/Items/Item.h"
#include "ItemContainerComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RUNEMAGIC_API UInventoryBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Items)
	static void CreateItemStackFromClass(TSubclassOf<UItem> ItemClass, int32 Count, FItemStack& ItemStack);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Items)
	static FString ItemStackDisplayName(FItemStack ItemStack);

	/** Calculates the missing number of items until the stack is full in the specified inventory */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Items)
	static int32 RoomInStack(UItemContainerComponent* Inventory, const FItemStack ItemStack);
};
