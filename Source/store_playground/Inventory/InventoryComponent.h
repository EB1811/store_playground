// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "InventoryComponent.generated.h"

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
  UPROPERTY(EditAnywhere, Category = "Inventory")
  EInventoryType InventoryType;
  UPROPERTY(EditAnywhere, Category = "Inventory")
  int32 MaxSlots;

  UPROPERTY(EditAnywhere, Category = "Inventory TESTING | Init State")
  TArray<FDataTableRowHandle> InitItemIds;

  void AddItem(const UItemBase* Item, int32 Quantity = 1);
  void RemoveItem(const UItemBase* Item, int32 Quantity = 1);
};

USTRUCT()
struct FInventoryTransferRes {
  GENERATED_BODY()

  bool bSuccess;
};
FInventoryTransferRes TransferItem(UInventoryComponent* From,
                                   UInventoryComponent* To,
                                   UItemBase* Item,
                                   int32 Quantity = 1);