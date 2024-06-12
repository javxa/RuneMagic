


#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Capacity = 20;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Yellow, FString::Printf(TEXT("PostInitProperties: %d"), Capacity));

	Items.SetNum(0);
	for (int i = 0; i < Capacity; i++)
	{
		Items.Add(FItemStack());
	}
}

FItemStack UInventoryComponent::AddItemStack(const FItemStack& ItemStack)
{
	return this->AddItems(ItemStack.ItemType, ItemStack.Count);
}

FItemStack UInventoryComponent::AddItems(UItem* Item, int32 Count)
{
	if (!Item || Count <= 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, TEXT("Item was null"));
		return FItemStack();
	}
	
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

		// Place new stack
		const int32 ToAdd = std::min(Count, Item->StackSize);
		Count -= ToAdd;

		const FItemStack NewStack = { Item, ToAdd };
		
		Items[i] = NewStack;
	}
	
	OnInventoryUpdated.Broadcast();

	if (Count > 0)
	{
		const FItemStack LeftOvers = { Item, Count };
		return LeftOvers;
	}

	return FItemStack();
}

bool UInventoryComponent::ContainsItems(UItem* Type, int32 Amount)
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

bool UInventoryComponent::RemoveItems(UItem* Item, int32 Count)
{
	if (!ContainsItems(Item, Count))
		return false;

	// Iterate backwards
	for (int i = Items.Num() - 1; i >= 0; i--)
	{
		FItemStack& It = Items[i];

		if (!It.ItemType->Equals(Item))
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

int32 UInventoryComponent::GetStackSize(const UItem* Item)
{
	return Item->StackSize;
}
