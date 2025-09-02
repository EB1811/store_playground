#include "MiniGameManager.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Minigame/MinigameWidgets/LootboxMinigame.h"
#include "store_playground/Minigame/MiniGameComponent.h"

AMiniGameManager::AMiniGameManager() { PrimaryActorTick.bCanEverTick = false; }

void AMiniGameManager::BeginPlay() {
  Super::BeginPlay();

  for (const auto& MiniGame : TEnumRange<EMiniGame>()) check(MiniGameInfoMap.Contains(MiniGame));

  check(LootboxMinigameClass);
  LootboxMinigameWidget = CreateWidget<ULootboxMinigame>(GetWorld(), LootboxMinigameClass);
}

void AMiniGameManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AMiniGameManager::GetMiniGameWidget(FInUIInputActions InUIInputActions,
                                         class UMiniGameComponent* MiniGameC,
                                         UInventoryComponent* PlayerInventory,
                                         std::function<void(class UUserWidget*)> CloseWidgetFunc)
    -> class UUserWidget* {
  check(PlayerInventory);

  auto UpdatePersistentDataFunc = [this](TMap<FName, float> AdditionalData) { PersistentData.Append(AdditionalData); };

  switch (MiniGameC->MiniGameType) {
    case EMiniGame::Lootbox: {
      check(LootboxMinigameWidget);

      LootboxMinigameWidget->InitUI(InUIInputActions, Market, MiniGameC, Store, PlayerInventory, PersistentData,
                                    UpdatePersistentDataFunc, CloseWidgetFunc);
      return LootboxMinigameWidget;
    }
    default: checkNoEntry(); return nullptr;
  }
}