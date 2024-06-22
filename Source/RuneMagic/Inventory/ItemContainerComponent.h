

#pragma once

#include "CoreMinimal.h"
#include "RuneMagic/Items/Item.h"
#include "RuneMagic/Items/ItemStack.h"
#include "Components/ActorComponent.h"
#include "ItemContainerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNEMAGIC_API UItemContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemContainerComponent();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Inventory)
	int32 Capacity;

	UPROPERTY(BlueprintReadOnly)
	TArray<FItemStack> Items;

	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FOnInventoryUpdated OnInventoryUpdated;
	
	/** Returns the items that were unable to be added */
	UFUNCTION(BlueprintCallable)
	FItemStack AddItemStack(const FItemStack& ItemStack);

	/** Returns the items that were unable to be added */
	UFUNCTION(BlueprintCallable)
	FItemStack AddItems(UItem* Item, int32 Count);

	UFUNCTION(BlueprintCallable)
	void SetItemAt(int32 Index, FItemStack ItemStack);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ContainsItems(UItem* Item, int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool RemoveItems(UItem* Item, int32 Count);

	/** Another inventory is pulling [Count] items, remove them from this inventory and return how many was removed */
	UFUNCTION(BlueprintCallable)
	int32 Pull(UItem* Item, int32 Count);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItemStack ItemAt(const int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool AcceptsItem(const UItem* Item);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	int32 GetStackSize(const UItem* Item);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
};
