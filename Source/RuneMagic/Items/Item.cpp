


#include "Item.h"

bool UItem::Equals(const UItem* Other) const
{
	return GetClass() == Other->GetClass();
}
