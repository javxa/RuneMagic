#include "InventoryWidgetManager.h"

#include "InventoryBPFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "InventoryOwnerWidget.h"
#include "RuneMagic/Items/Item.h"
#include "RuneMagic/Items/ItemStack.h"

UInventoryWidgetManager::UInventoryWidgetManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	PlayerController = nullptr;
}

void UInventoryWidgetManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());

	if (!PlayerController && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, TEXT("Owner of InventoryWidgetManager should be a PlayerController"));
}

void UInventoryWidgetManager::OpenInventory(const TScriptInterface<IInventoryOwner> InventoryOwner)
{
	if (!PlayerController) return;
	
	const EInventoryType Type = IInventoryOwner::Execute_GetType(InventoryOwner.GetObject());
	const bool WasOpen = IsInventoryOpen(InventoryOwner);
	
	CloseInventoryType(Type);

	// Toggle-like if trying to open the same inventory again
	if (WasOpen) return;
	
	UInventoryOwnerWidget* Widget = IInventoryOwner::Execute_CreateInventoryWidget(InventoryOwner.GetObject(), PlayerController);
	Widget->InventoryWidgetManager = this;
	
	Widget->AddToViewport();

	OpenWidgetMap.Add(Type, { Widget, InventoryOwner });
}

bool UInventoryWidgetManager::CloseInventory(const TScriptInterface<IInventoryOwner> InventoryOwner)
{
	const EInventoryType Type = IInventoryOwner::Execute_GetType(InventoryOwner.GetObject());
	if (!IsInventoryOpen(InventoryOwner)) return false;
	return CloseInventoryType(Type);
}

bool UInventoryWidgetManager::CloseInventoryType(const EInventoryType Type)
{
	if (!IsTypeOpen(Type))
		return false;
	
	const auto [Widget, InventoryOwner] = OpenWidgetMap.FindAndRemoveChecked(Type);

	Widget->RemoveFromParent();
	return true;
}

bool UInventoryWidgetManager::CloseAllInventories()
{
	bool AnyClosed = false;

	TArray<EInventoryType> OpenTypes;
	OpenWidgetMap.GetKeys(OpenTypes);

	for (const EInventoryType& Type : OpenTypes)
	{
		AnyClosed |= CloseInventoryType(Type);
	}
	
	return AnyClosed;
}

bool UInventoryWidgetManager::IsInventoryOpen(const TScriptInterface<IInventoryOwner> InventoryOwner)
{
	const EInventoryType Type = IInventoryOwner::Execute_GetType(InventoryOwner.GetObject());
	const FOpenInventory* Inv = OpenWidgetMap.Find(Type);
	
	if (!Inv) return false;
	return Inv->InventoryOwner == InventoryOwner;
}

bool UInventoryWidgetManager::IsTypeOpen(const EInventoryType Type) const
{
	return OpenWidgetMap.Contains(Type);
}

void UInventoryWidgetManager::ItemClicked(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType)
{
	switch (ClickType)
	{
	case EItemClickType::TakeAll:
	case EItemClickType::TakeHalf:
		TakePlaceItem(InWidget, Inventory, ItemIndex, ClickType);
		break;
	case EItemClickType::PushAll:
	case EItemClickType::PushOne:
		PushItem(InWidget, Inventory, ItemIndex, ClickType);
		break;
	case EItemClickType::PullOne:
		PullItem(InWidget, Inventory, ItemIndex, ClickType);
		break;
	}
}

void UInventoryWidgetManager::RegisterCursorInventory(UItemContainerComponent* ItemContainer)
{
	this->CursorContainer = ItemContainer;
}

void UInventoryWidgetManager::TakePlaceItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType)
{
	if (!CursorContainer) return;
	
	const FItemStack CursorItem = CursorContainer->ItemAt(0);
	const FItemStack InventoryItem = Inventory->ItemAt(ItemIndex);

	if (!CursorItem.Count && !InventoryItem.Count)
		return;
	
	if (ClickType == EItemClickType::TakeAll)
	{
		// Place all
		if (CursorItem.Count && InventoryItem.Count && CursorItem.ItemType->Equals(InventoryItem.ItemType))
		{
			MoveItemsSafe(CursorContainer, 0, Inventory, ItemIndex, CursorItem.Count);
			return;
		}

		// Take all / Switch
		SwitchItemsSafe(Inventory, ItemIndex, CursorContainer, 0);
		return;
	}

	// Take half
	if (!CursorItem.Count)
	{
		const int32 TakeAmount = std::max(1, InventoryItem.Count / 2);
		MoveItemsSafe(Inventory, ItemIndex, CursorContainer, 0, TakeAmount);
		return;
	}
	
	const bool couldMove = MoveItemsSafe(CursorContainer, 0, Inventory, ItemIndex, 1);

	if (!couldMove)
	{
		SwitchItemsSafe(Inventory, ItemIndex, CursorContainer, 0);
	}
}

bool UInventoryWidgetManager::SwitchItemsSafe(UItemContainerComponent* ContainerA, int32 IndexA,
	UItemContainerComponent* ContainerB, int32 IndexB)
{
	const FItemStack ItemA = ContainerA->ItemAt(IndexA);
	const FItemStack ItemB = ContainerB->ItemAt(IndexB);

	if (!ItemA.Count && !ItemB.Count)
		return false;

	// Simplify handling by delegating to safe move if one of the slots are empty
	if (!ItemA.Count)
		return MoveItemsSafe(ContainerB, IndexB, ContainerA, IndexA, ItemB.Count);

	if (!ItemB.Count)
		return MoveItemsSafe(ContainerA, IndexA, ContainerB, IndexB, ItemA.Count);

	// There are items in both slots, make sure both inventories can store the other item
	if (!ContainerB->AcceptsItem(ItemA.ItemType))
		return false;
	
	if (ItemA.Count > ContainerB->GetStackSize(ItemA.ItemType))
		return false;

	if (!ContainerA->AcceptsItem(ItemB.ItemType))
		return false;

	if (ItemB.Count > ContainerA->GetStackSize(ItemB.ItemType))
		return false;

	// Everything is good to go, switch items
	
	ContainerB->SetItemAt(IndexB, ItemA);
	ContainerA->SetItemAt(IndexA, ItemB);

	return true;
}

bool UInventoryWidgetManager::MoveItemsSafe(UItemContainerComponent* ContainerFrom, int32 IndexFrom, UItemContainerComponent* ContainerTo, int32 IndexTo, int32 Count)
{
	if (!Count) return false;
	
	const FItemStack ItemFrom = ContainerFrom->ItemAt(IndexFrom);

	if (!ItemFrom.Count)
		return false;
	
	const FItemStack ItemTo = ContainerTo->ItemAt(IndexTo);
	
	int32 MoveAmount = std::min(ItemFrom.Count, Count);

	if (!ItemTo.Count)
	{
		if (!ContainerTo->AcceptsItem(ItemFrom.ItemType))
			return false;
		
		const int32 StackSize = ContainerTo->GetStackSize(ItemFrom.ItemType);
		MoveAmount = std::min(MoveAmount, StackSize);

		const int32 Left = ItemFrom.Count - MoveAmount;
		ContainerTo->SetItemAt(IndexTo, {ItemFrom.ItemType, MoveAmount});

		if (Left == 0)
		{
			ContainerFrom->SetItemAt(IndexFrom, {nullptr, Left});
		} else
		{
			ContainerFrom->SetItemAt(IndexFrom, {ItemFrom.ItemType, Left});
		}
		
		return true;
	}

	if (!ItemTo.ItemType->Equals(ItemFrom.ItemType))
		return false;

	MoveAmount = std::min(MoveAmount, UInventoryBPFunctionLibrary::RoomInStack(ContainerTo, ItemTo));
	
	if (!MoveAmount)
		return false;

	ContainerTo->SetItemAt(IndexTo, {ItemFrom.ItemType, ItemTo.Count + MoveAmount});
	const int32 Left = ItemFrom.Count - MoveAmount;
	
	if (Left == 0)
	{
		ContainerFrom->SetItemAt(IndexFrom, {nullptr, Left});
	} else
	{
		ContainerFrom->SetItemAt(IndexFrom, {ItemFrom.ItemType, Left});
	}
	
	return true;

}

void UInventoryWidgetManager::PullItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType)
{
	if (!Inventory) return;
	if (!InWidget) return;
	
	const FItemStack Item = Inventory->ItemAt(ItemIndex);
	
	if (!Item.Count)
		return;

	UItemContainerComponent* TargetInventory = FindOpenTargetInventory(InWidget, Inventory);

	if (!TargetInventory)
		return;
	
	if (Inventory == TargetInventory)
		return;
	
	const int32 MoveAmount = std::min(1, UInventoryBPFunctionLibrary::RoomInStack(Inventory, Item));

	if (MoveAmount == 0) return;
	
	const int32 Pulled = TargetInventory->Pull(Item.ItemType, MoveAmount);
	if (Pulled == 0) return;

	Inventory->SetItemAt(ItemIndex, {Item.ItemType, Item.Count + Pulled});
}

void UInventoryWidgetManager::PushItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType)
{
	if (!Inventory) return;
	if (!InWidget) return;
	
	const FItemStack Item = Inventory->ItemAt(ItemIndex);
	
	if (!Item.Count)
		return;
	
	UItemContainerComponent* TargetInventory = FindOpenTargetInventory(InWidget, Inventory);

	if (!TargetInventory)
		return;
	
	if (Inventory == TargetInventory)
		return;
	
	const int32 MoveAmount = ClickType == EItemClickType::PushAll ? Item.Count : 1;
	
	const FItemStack LeftOvers = TargetInventory->AddItems(Item.ItemType, MoveAmount);
	const int32 AmountMoved = MoveAmount - LeftOvers.Count;

	Inventory->SetItemAt(ItemIndex, {Item.ItemType, Item.Count - AmountMoved});
}

UItemContainerComponent* UInventoryWidgetManager::FindOpenTargetInventory(UInventoryOwnerWidget* InWidget, UItemContainerComponent* SourceInventory) const
{
	TArray<FOpenInventory> OpenInventories;
	OpenWidgetMap.GenerateValueArray(OpenInventories);

	// If only one inventory is open
	if (OpenInventories.Num() == 1)
	{
		// Look for a target item container
		FOpenInventory TargetOpenInventory = OpenInventories[0];
		
		return IInventoryOwner::Execute_GetTargetInventory(TargetOpenInventory.InventoryOwner.GetObject(), SourceInventory);
	}
	
	// Filter out source widget
	OpenInventories.RemoveAll([InWidget](const FOpenInventory& Inv)
	{
		return Inv.Widget == InWidget;
	});

	for (auto& OpenInventory : OpenInventories)
	{
		if (UItemContainerComponent* PotentialTarget = IInventoryOwner::Execute_GetTargetInventory(OpenInventory.InventoryOwner.GetObject(), SourceInventory))
			return PotentialTarget;
	}

	return nullptr;
}
