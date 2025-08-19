// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "InventoryComponent.generated.h"

UENUM()
enum class EInventoryType : uint8 {
  Container UMETA(DisplayName = "Container"),
  Store UMETA(DisplayName = "Store"),                      // Items are not removed.
  StockDisplay UMETA(DisplayName = "StockDisplay"),        // Only single items.
  PlayerInventory UMETA(DisplayName = "PlayerInventory"),  // Item equality defined by BoughtAt price.
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInventoryComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInventoryComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Inventory")
  TArray<TObjectPtr<UItemBase>> ItemsArray;
  UPROPERTY(EditAnywhere, Category = "Inventory", SaveGame)
  EInventoryType InventoryType;
  UPROPERTY(EditAnywhere, Category = "Inventory", SaveGame)
  int32 MaxSlots;

#if WITH_EDITORONLY_DATA
  UPROPERTY(EditAnywhere, Category = "Inventory TESTING | Init State")
  bool bInitWithTestItems;
  UPROPERTY(EditAnywhere, Category = "Inventory TESTING | Init State")
  TArray<FDataTableRowHandle> InitItemIds;
#endif  // WITH_EDITORONLY_DATA

  auto AddItem(const UItemBase* Item, int32 Quantity = 1) -> UItemBase*;
  void RemoveItem(const UItemBase* Item, int32 Quantity = 1);
};

auto AreItemsEqual(EInventoryType InventoryType, const UItemBase* Item1, const UItemBase* Item2) -> bool;

auto CanTransferItem(const UInventoryComponent* To, const UItemBase* Item) -> bool;

USTRUCT()
struct FInventoryTransferRes {
  GENERATED_BODY()

  bool bSuccess;
  UItemBase* ItemCopy;  // Since a new copy is created, this is the new item reference.
};
auto TransferItem(UInventoryComponent* From,
                  UInventoryComponent* To,
                  UItemBase* Item,
                  int32 Quantity = 1) -> FInventoryTransferRes;
