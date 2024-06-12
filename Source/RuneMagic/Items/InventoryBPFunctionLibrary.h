

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStack.h"
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
};
