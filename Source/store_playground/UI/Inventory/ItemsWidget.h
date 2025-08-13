// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "ItemsWidget.generated.h"

UENUM()
enum class ESortType : uint8 {
  None UMETA(DisplayName = "None"),
  Price UMETA(DisplayName = "Price"),
  Name UMETA(DisplayName = "Name"),
};
USTRUCT()
struct FSortData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  ESortType SortType;

  UPROPERTY(EditAnywhere)
  bool bReverse;
};
USTRUCT()
struct FItemsFilterData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EItemType ItemType;

  UPROPERTY(EditAnywhere)
  bool bFilterByType;
};

UCLASS()
class STORE_PLAYGROUND_API UItemsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* ItemsPanelWrapBox;
  UPROPERTY(meta = (BindWidget))
  class UItemDetailsWidget* ItemDetailsWidget;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NoItemsText;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UItemSlotWidget> ItemSlotWidgetClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* SelectSound;

  UPROPERTY(EditAnywhere)
  const class UInventoryComponent* InventoryRef;

  UPROPERTY(EditAnywhere)
  TArray<const class UItemBase*> SortedItems;

  UPROPERTY(EditAnywhere)
  const class UItemBase* SelectedItem;
  UPROPERTY(EditAnywhere)
  class UItemSlotWidget* SelectedItemSlotWidget;

  UPROPERTY(EditAnywhere)
  FSortData SortData;
  UPROPERTY(EditAnywhere)
  FItemsFilterData FilterData;

  FName ShowPriceText;
  std::function<float(FName)> ShowPriceFunc;
  std::function<float(FName)> MarketPriceFunc;

  void SelectItem(const class UItemBase* Item, UItemSlotWidget* ItemSlotWidget);
  void SelectNextItem(FVector2D Direction);

  void SortItems(FSortData _SortData);
  void FilterItems(FItemsFilterData _FilterData);

  void RefreshUI();
  void InitUI(const class UInventoryComponent* _InventoryRef,
              FName _ShowPriceText,
              std::function<float(FName)> _MarketPriceFunc,
              std::function<float(FName)> _ShowPriceFunc = nullptr);
};