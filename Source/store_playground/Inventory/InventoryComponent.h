// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "InventoryComponent.generated.h"

// TODO: Adjust item equality to be based on bought price.
// TODO: Change all instances of item equality to use this.

UENUM()
enum class EInventoryType : uint8 {
  Container UMETA(DisplayName = "Container"),
  Store UMETA(DisplayName = "Store"),                // Items are not removed.
  StockDisplay UMETA(DisplayName = "StockDisplay"),  // Only single items.
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

  UPROPERTY(EditAnywhere, Category = "Inventory TESTING | Init State")
  TArray<FDataTableRowHandle> InitItemIds;

  auto AddItem(const UItemBase* Item, int32 Quantity = 1) -> UItemBase*;
  void RemoveItem(const UItemBase* Item, int32 Quantity = 1);
};

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
