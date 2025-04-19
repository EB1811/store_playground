#include "MiniGameManager.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Minigame/MinigameWidgets/LootboxMinigame.h"

AMiniGameManager::AMiniGameManager() { PrimaryActorTick.bCanEverTick = false; }

void AMiniGameManager::BeginPlay() {
  Super::BeginPlay();

  for (const auto& MiniGame : TEnumRange<EMiniGame>()) check(MiniGameInfoMap.Contains(MiniGame));

  check(LootboxMinigameClass);
  LootboxMinigameWidget = CreateWidget<ULootboxMinigame>(GetWorld(), LootboxMinigameClass);
}

void AMiniGameManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AMiniGameManager::GetMiniGameWidget(EMiniGame MiniGame,
                                         AStore* Store,
                                         UInventoryComponent* PlayerInventory,
                                         std::function<void(class UUserWidget*)> CloseMinigameFunc)
    -> class UUserWidget* {
  check(Market && Store && PlayerInventory);
  check(MiniGameInfoMap.Contains(MiniGame));

  std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc = [this](TMap<FName, float> AdditionalData) {
    PersistentData.Append(AdditionalData);
  };
  const TMap<EMiniGame, TTuple<UUserWidget*, MiniGameT>> MiniGameMap = {
      {EMiniGame::Lootbox, {LootboxMinigameWidget, LootboxMinigameWidget}}};

  MiniGameMap[MiniGame].Value.InitMiniGame(Market, Store, PlayerInventory, PersistentData, UpdatePersistentDataFunc,
                                           CloseMinigameFunc);
  return MiniGameMap[MiniGame].Key;

  // FMiniGameInfo& MiniGameInfo = MiniGameInfoMap[MiniGame];
  // UUserWidget* GameWidget = CreateWidget<UUserWidget>(GetWorld(), MiniGameInfo.GameWidgetClass);
  // check(GameWidget);
  // Cast<IMiniGameInterface>(GameWidget)
  //     ->InitMiniGame(Market, Store, PlayerInventory, PersistentData, UpdatePersistentDataFunc, CloseMinigameFunc);
  // return GameWidget;
}