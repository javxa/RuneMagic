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
	
	FItemStack CursorItem = CursorContainer->ItemAt(0);
	FItemStack InventoryItem = Inventory->ItemAt(ItemIndex);

	if (!CursorItem.Count && !InventoryItem.Count)
		return;
	
	if (ClickType == EItemClickType::TakeAll)
	{
		// Fill whole stack
		if (CursorItem.Count && InventoryItem.Count && CursorItem.ItemType->Equals(InventoryItem.ItemType))
		{
			const int32 PlaceCount = std::min(CursorItem.Count, UInventoryBPFunctionLibrary::RoomInStack(Inventory, InventoryItem));
			if (PlaceCount == 0)
				return;
			Inventory->SetItemAt(ItemIndex, {InventoryItem.ItemType, InventoryItem.Count + PlaceCount});
			CursorContainer->SetItemAt(0, {CursorItem.ItemType, CursorItem.Count - PlaceCount});
			return;
		}
		
		// Switch
		Inventory->SetItemAt(ItemIndex, CursorItem);
		CursorContainer->SetItemAt(0, InventoryItem);
		return;
	}

	// Take half
	if (!CursorItem.Count)
	{
		const int32 TakeAmount = std::max(1, InventoryItem.Count / 2);
		Inventory->SetItemAt(ItemIndex, {InventoryItem.ItemType, InventoryItem.Count - TakeAmount});
		CursorContainer->SetItemAt(0, {InventoryItem.ItemType, TakeAmount});
		return;
	}

	int32 PlaceCount = std::min(1, CursorItem.Count);
	
	// Place one
	if (!InventoryItem.Count)
	{
		Inventory->SetItemAt(ItemIndex, {CursorItem.ItemType, PlaceCount});
		CursorContainer->SetItemAt(0, {CursorItem.ItemType, CursorItem.Count - PlaceCount});
		return;
	}
	
	// Fill one
	if (CursorItem.ItemType->Equals(InventoryItem.ItemType))
	{
		PlaceCount = std::min(PlaceCount, UInventoryBPFunctionLibrary::RoomInStack(Inventory, InventoryItem));
		if (PlaceCount == 0)
			return;
		Inventory->SetItemAt(ItemIndex, {InventoryItem.ItemType, InventoryItem.Count + PlaceCount});
		CursorContainer->SetItemAt(0, {CursorItem.ItemType, CursorItem.Count - PlaceCount});
		return;
	}

	// Items not matching, switch (again)
	Inventory->SetItemAt(ItemIndex, CursorItem);
	CursorContainer->SetItemAt(0, InventoryItem);
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

	const int32 StackSize = Inventory->GetStackSize(Item.ItemType);
	const int32 SpaceInStack = std::max(0, StackSize - Item.Count);
	const int32 MoveAmount = std::min(1, SpaceInStack);

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
