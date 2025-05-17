#include "LootboxMinigame.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Store/Store.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void ULootboxMinigame::NativeOnInitialized() {
  Super::NativeOnInitialized();

  OpenLootboxButton->OnClicked.AddDynamic(this, &ULootboxMinigame::OnOpenLootboxButtonClicked);

  ResultText->SetVisibility(ESlateVisibility::Hidden);
  ItemIcon->SetVisibility(ESlateVisibility::Hidden);
  ItemGivenText->SetVisibility(ESlateVisibility::Hidden);
}

void ULootboxMinigame::OnOpenLootboxButtonClicked() {
  check(Market && PlayerInventory);

  PriceText->SetVisibility(ESlateVisibility::Hidden);
  OpenLootboxButton->SetVisibility(ESlateVisibility::Hidden);
  ResultText->SetVisibility(ESlateVisibility::Visible);
  ItemIcon->SetVisibility(ESlateVisibility::Visible);
  ItemGivenText->SetVisibility(ESlateVisibility::Visible);

  if (!Store->TrySpendMoney(Price)) {
    ResultText->SetText(FText::FromString("Not enough money!"));
    return;
  }

  float WinChance =
      LootboxMinigameParams.BaseWinChance *
      (Price / (LootboxMinigameParams.BaseLootboxPriceMin + LootboxMinigameParams.BaseLootboxPriceMax / 2));
  if (FMath::FRand() * 100 >= LootboxMinigameParams.BaseWinChance) {
    ResultText->SetText(FText::FromString("You lost!"));
    return;
  }
  ResultText->SetText(FText::FromString("You won!"));

  UItemBase* Item = Market->GetNewRandomItems(1)[0];
  PlayerInventory->AddItem(Item, 1);
  ItemIcon->SetBrushFromTexture(Item->AssetData.Icon);
  ItemGivenText->SetText(Item->TextData.Name);
}

void ULootboxMinigame::InitMiniGame(const class AMarket* _Market,
                                    class AStore* _Store,
                                    class UInventoryComponent* _PlayerInv,
                                    TMap<FName, float> _PersistentData,
                                    std::function<void(TMap<FName, float>)> _UpdatePersistentDataFunc,
                                    std::function<void(class UUserWidget*)> _CloseMinigameFunc) {
  Market = _Market;
  Store = _Store;
  PlayerInventory = _PlayerInv;
  PersistentData = _PersistentData;
  UpdatePersistentDataFunc = _UpdatePersistentDataFunc;
  CloseMinigameFunc = _CloseMinigameFunc;

  Price = FMath::FRandRange(LootboxMinigameParams.BaseLootboxPriceMin, LootboxMinigameParams.BaseLootboxPriceMax);
  PriceText->SetText(FText::FromString(FString::FromInt(Price)));

  PriceText->SetVisibility(ESlateVisibility::Visible);
  OpenLootboxButton->SetVisibility(ESlateVisibility::Visible);
  ResultText->SetVisibility(ESlateVisibility::Hidden);
  ItemIcon->SetVisibility(ESlateVisibility::Hidden);
  ItemGivenText->SetVisibility(ESlateVisibility::Hidden);
}