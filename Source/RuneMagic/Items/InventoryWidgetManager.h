#pragma once
#include "InventoryOwner.h"
#include "InventoryType.h"

#include "InventoryWidgetManager.generated.h"

USTRUCT(BlueprintType)
struct RUNEMAGIC_API FOpenInventory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=OpenInventory)
	TObjectPtr<UUserWidget> Widget;

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

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY()
	TMap<EInventoryType, FOpenInventory> OpenWidgetMap;
};


