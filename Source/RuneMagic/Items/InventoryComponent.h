

#pragma once

#include "CoreMinimal.h"
#include "ItemStack.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNEMAGIC_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

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
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Inventory)
	int32 Capacity;
	
	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintReadOnly)
	TArray<FItemStack> Items;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual int32 GetStackSize(const UItem* Item);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
};
