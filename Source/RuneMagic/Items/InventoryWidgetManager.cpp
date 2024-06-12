#include "InventoryWidgetManager.h"

#include "Blueprint/UserWidget.h"

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
	
	UUserWidget* UserWidget = IInventoryOwner::Execute_CreateInventoryWidget(InventoryOwner.GetObject(), PlayerController);
	
	UserWidget->AddToViewport();

	OpenWidgetMap.Add(Type, { UserWidget, InventoryOwner });
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
