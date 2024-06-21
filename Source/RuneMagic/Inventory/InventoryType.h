#pragma once

#include "InventoryType.generated.h"

UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	Player UMETA(DisplayName="Player"),
	Target UMETA(DisplayName="Target")
};