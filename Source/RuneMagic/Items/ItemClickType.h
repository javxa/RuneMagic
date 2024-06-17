#pragma once

#include "ItemClickType.generated.h"

UENUM(BlueprintType)
enum class EItemClickType : uint8
{
	TakeAll,
	TakeHalf,
	PushAll,
	PushOne,
	PullOne
};