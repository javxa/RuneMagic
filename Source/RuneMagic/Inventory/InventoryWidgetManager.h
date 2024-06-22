#pragma once
#include "ItemContainerComponent.h"
#include "InventoryOwner.h"
#include "InventoryType.h"
#include "ItemClickType.h"

#include "InventoryWidgetManager.generated.h"

USTRUCT(BlueprintType)
struct RUNEMAGIC_API FOpenInventory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=OpenInventory)
	TObjectPtr<UInventoryOwnerWidget> Widget;

	UPROPERTY(BlueprintReadWrite, Category=OpenInventory)
	TScriptInterface<IInventoryOwner> InventoryOwner;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNEMAGIC_API UInventoryWidgetManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryWidgetManager();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void OpenInventory(TScriptInterface<IInventoryOwner> InventoryOwner);

	UFUNCTION(BlueprintCallable)
	bool CloseInventory(TScriptInterface<IInventoryOwner> InventoryOwner);

	UFUNCTION(BlueprintCallable)
	bool CloseInventoryType(const EInventoryType Type);

	UFUNCTION(BlueprintCallable)
	bool CloseAllInventories();

	UFUNCTION(BlueprintCallable)
	bool IsInventoryOpen(TScriptInterface<IInventoryOwner> InventoryOwner);

	UFUNCTION(BlueprintCallable)
	bool IsTypeOpen(EInventoryType Type) const;

	UFUNCTION(BlueprintCallable)
	void ItemClicked(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType);

	UFUNCTION(BlueprintCallable)
	void RegisterCursorInventory(UItemContainerComponent* ItemContainer);

private:
	UPROPERTY()
	TObjectPtr<UItemContainerComponent> CursorContainer;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY()
	TMap<EInventoryType, FOpenInventory> OpenWidgetMap;

	UFUNCTION()
	void PushItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType);

	UFUNCTION()
	void PullItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType);

	UFUNCTION()
	void TakePlaceItem(UInventoryOwnerWidget* InWidget, UItemContainerComponent* Inventory, int32 ItemIndex, EItemClickType ClickType);

	UFUNCTION()
	bool SwitchItemsSafe(UItemContainerComponent* ContainerA, int32 IndexA, UItemContainerComponent* ContainerB, int32 IndexB);

	UFUNCTION()
	bool MoveItemsSafe(UItemContainerComponent* ContainerFrom, int32 IndexFrom, UItemContainerComponent* ContainerTo, int32 IndexTo, int32 Count);

	UFUNCTION()
	UItemContainerComponent* FindOpenTargetInventory(UInventoryOwnerWidget* InWidget, UItemContainerComponent* SourceInventory) const;
};


