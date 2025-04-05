// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Framework/StorePGGameMode.h"
#include "Engine/World.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  check(PlayerCharacter && StorePhaseManagerClass && CustomerAIManagerClass && QuestManagerClass && MarketClass &&
        MarketEconomyClass && StoreClass);

  // * Initialize the game world and all systems.
  ALevelManager* LevelManager = GetWorld()->SpawnActor<ALevelManager>(LevelManagerClass);
  AUpgradeManager* UpgradeManager = GetWorld()->SpawnActor<AUpgradeManager>(UpgradeManagerClass);
  AGlobalDataManager* GlobalDataManager = GetWorld()->SpawnActor<AGlobalDataManager>(GlobalDataManagerClass);
  AStorePhaseManager* StorePhaseManager = GetWorld()->SpawnActor<AStorePhaseManager>(StorePhaseManagerClass);
  ADayManager* DayManager = GetWorld()->SpawnActor<ADayManager>(DayManagerClass);
  ACustomerAIManager* CustomerAIManager = GetWorld()->SpawnActor<ACustomerAIManager>(CustomerAIManagerClass);
  AQuestManager* QuestManager = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass);
  AMarket* Market = GetWorld()->SpawnActor<AMarket>(MarketClass);
  AMarketEconomy* MarketEconomy = GetWorld()->SpawnActor<AMarketEconomy>(MarketEconomyClass);
  ANewsGen* NewsGen = GetWorld()->SpawnActor<ANewsGen>(NewsGenClass);
  AStore* Store = GetWorld()->SpawnActor<AStore>(StoreClass);
  UDialogueSystem* DialogueSystem = NewObject<UDialogueSystem>(this);
  UNegotiationSystem* NegotiationSystem = NewObject<UNegotiationSystem>(this);

  PlayerCharacter->DialogueSystem = DialogueSystem;
  PlayerCharacter->NegotiationSystem = NegotiationSystem;
  PlayerCharacter->LevelManager = LevelManager;
  PlayerCharacter->Store = Store;
  PlayerCharacter->StorePhaseManager = StorePhaseManager;
  PlayerCharacter->DayManager = DayManager;
  PlayerCharacter->Market = Market;
  PlayerCharacter->MarketEconomy = MarketEconomy;
  PlayerCharacter->CustomerAIManager = CustomerAIManager;
  PlayerCharacter->QuestManager = QuestManager;
  PlayerCharacter->NewsGen = NewsGen;
  PlayerCharacter->UpgradeManager = UpgradeManager;

  UpgradeManager->CustomerAIManager = CustomerAIManager;
  UpgradeManager->Market = Market;
  UpgradeManager->GlobalDataManager = GlobalDataManager;

  GlobalDataManager->PlayerCharacter = PlayerCharacter;

  LevelManager->Store = Store;
  LevelManager->Market = Market;

  StorePhaseManager->DayManager = DayManager;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;

  DayManager->CustomerAIManager = CustomerAIManager;
  DayManager->MarketEconomy = MarketEconomy;
  DayManager->Market = Market;
  DayManager->NewsGen = NewsGen;

  CustomerAIManager->GlobalDataManager = GlobalDataManager;
  CustomerAIManager->Market = Market;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;
  CustomerAIManager->QuestManager = QuestManager;

  QuestManager->GlobalDataManager = GlobalDataManager;

  Market->GlobalDataManager = GlobalDataManager;
  Market->QuestManager = QuestManager;
  Market->MarketEconomy = MarketEconomy;

  NewsGen->GlobalDataManager = GlobalDataManager;

  NegotiationSystem->MarketEconomy = MarketEconomy;
  NegotiationSystem->DialogueSystem = DialogueSystem;
  NegotiationSystem->Store = Store;
  NegotiationSystem->PlayerInventory = PlayerCharacter->PlayerInventoryComponent;
  NegotiationSystem->QuestManager = QuestManager;

  LevelManager->BeginLoadLevel(ELevel::Store);
  StorePhaseManager->Start();

  UE_LOG(LogTemp, Warning, TEXT("Game Initialized."));

  // * Clearing datatable refs mostly.
  CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
}