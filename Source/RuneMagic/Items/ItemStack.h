

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "UObject/NoExportTypes.h"
#include "ItemStack.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RUNEMAGIC_API FItemStack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Item)
	UItem* ItemType;

	UPROPERTY(BlueprintReadWrite, Category=Item)
	int32 Count;
};
