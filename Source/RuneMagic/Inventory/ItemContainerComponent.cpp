#include "ItemContainerComponent.h"
#include "RuneMagic/Items/Item.h"
#include "RuneMagic/Items/ItemStack.h"
#include "RuneMagic/Log.h"

UItemContainerComponent::UItemContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Capacity = 20;
}

void UItemContainerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UItemContainerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Items.SetNum(0);
	for (int i = 0; i < Capacity; i++)
	{
		Items.Add(FItemStack());
	}
}

FItemStack UItemContainerComponent::AddItemStack(const FItemStack& ItemStack)
{
	return this->AddItems(ItemStack.ItemType, ItemStack.Count);
}

FItemStack UItemContainerComponent::AddItems(UItem* Item, int32 Count)
{
	if (!Item || Count <= 0)
	{
		ERROR("UItemContainerComponent::AddItems Tried to add null item or 0 count to inventory");
		return FItemStack();
	}

	// Outer loop to first fill existing slots, only after can we fill empty slots
	for (int32 fill_empty = 0; fill_empty <= 1; fill_empty++)
	{
		if (Count <= 0)
			break;
		
		for (int i = 0; i < Items.Num(); i++)
		{
			if (Count <= 0)
				break;

			FItemStack& It = Items[i];

			if (It.Count)
			{
				// Slot is not empty

				if (!Item->Equals(It.ItemType))
					continue;

				const int32 Missing = Item->StackSize - It.Count;
				const int32 ToAdd = std::min(Missing, Count);

				Count -= ToAdd;
				It.Count += ToAdd;

				continue;
			}

			if (!fill_empty)
				continue;

			// Place new stack
			const int32 ToAdd = std::min(Count, Item->StackSize);
			Count -= ToAdd;

			const FItemStack NewStack = {Item, ToAdd};

			Items[i] = NewStack;
		}
	}
	
	OnInventoryUpdated.Broadcast();

	if (Count > 0)
	{
		const FItemStack LeftOvers = {Item, Count};
		return LeftOvers;
	}

	return FItemStack();
}

bool UItemContainerComponent::ContainsItems(UItem* Type, int32 Amount)
{
	for (const auto& [ItemType, Count] : this->Items)
	{
		if (!Type->Equals(ItemType))
			continue;

		Amount -= Count;

		if (Count <= 0)
			return true;
	}

	return false;
}

bool UItemContainerComponent::RemoveItems(UItem* Item, int32 Count)
{
	if (!ContainsItems(Item, Count))
		return false;

	// Iterate backwards
	for (int i = Items.Num() - 1; i >= 0; i--)
	{
		FItemStack& It = Items[i];

		if (!It.ItemType || !It.ItemType->Equals(Item))
			continue;

		if (It.Count > Count)
		{
			It.Count -= Count;
			break;
		}

		Count -= It.Count;
		Items[i] = FItemStack();
	}

	OnInventoryUpdated.Broadcast();
	return true;
}

int32 UItemContainerComponent::Pull(UItem* Item, int32 Count)
{
	int32 Pulled = 0;

	// Iterate backwards
	for (int i = Items.Num() - 1; i >= 0; i--)
	{
		FItemStack& It = Items[i];

		if (!It.ItemType || !It.ItemType->Equals(Item))
			continue;

		if (It.Count > Count)
		{
			It.Count -= Count;
			Pulled += Count;
			break;
		}

		Count -= It.Count;
		Pulled += It.Count;
		Items[i] = FItemStack();
	}

	OnInventoryUpdated.Broadcast();
	return Pulled;
}

int32 UItemContainerComponent::GetStackSize(const UItem* Item)
{
	return Item->StackSize;
}

FItemStack UItemContainerComponent::ItemAt(const int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		ERROR("InventoryComponent.ItemAt Index out of bounds: %d", Index);
		return FItemStack();
	}

	return Items[Index];
}

void UItemContainerComponent::SetItemAt(const int32 Index, const FItemStack ItemStack)
{
	if (!Items.IsValidIndex(Index))
	{
		ERROR("InventoryComponent.SetItemAt Index out of bounds: %d", Index);
		return;
	}

	Items[Index] = ItemStack;
	OnInventoryUpdated.Broadcast();
}
