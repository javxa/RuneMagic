

#pragma once

#include "CoreMinimal.h"
#include "ItemStack.h"
#include "Components/ActorComponent.h"
#include "ItemContainerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNEMAGIC_API UItemContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemContainerComponent();

	/** Returns the items that were unable to be added */
	UFUNCTION(BlueprintCallable)
	FItemStack AddItemStack(const FItemStack& ItemStack);

	/** Returns the items that were unable to be added */
	UFUNCTION(BlueprintCallable)
	FItemStack AddItems(UItem* Item, int32 Count);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ContainsItems(UItem* Item, int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool RemoveItems(UItem* Item, int32 Count);

	/** Another inventory is pulling [Count] items, remove them from this inventory and return how many was removed */
	UFUNCTION(BlueprintCallable)
	int32 Pull(UItem* Item, int32 Count);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Inventory)
	int32 Capacity;
	
	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintReadOnly)
	TArray<FItemStack> Items;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual int32 GetStackSize(const UItem* Item);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItemStack ItemAt(const int32 Index);

	UFUNCTION(BlueprintCallable)
	void SetItemAt(int32 Index, FItemStack ItemStack);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
};
