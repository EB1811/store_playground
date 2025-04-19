// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Framework/StorePGGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  check(PlayerCharacter && LevelManagerClass && GlobalDataManagerClass && UpgradeManagerClass && SaveManagerClass &&
        PlayerCommandClass && StorePhaseManagerClass && DayManagerClass && CustomerAIManagerClass &&
        QuestManagerClass && MarketClass && MarketEconomyClass && NewsGenClass && StoreClass && MiniGameManagerClass &&
        AbilityManagerClass);

  // * Initialize the game world and all systems.
  UDialogueSystem* DialogueSystem = NewObject<UDialogueSystem>(this);
  UNegotiationSystem* NegotiationSystem = NewObject<UNegotiationSystem>(this);
  ALevelManager* LevelManager = GetWorld()->SpawnActor<ALevelManager>(LevelManagerClass);
  SaveManager = GetWorld()->SpawnActor<ASaveManager>(SaveManagerClass);
  APlayerCommand* PlayerCommand = GetWorld()->SpawnActor<APlayerCommand>(PlayerCommandClass);
  AUpgradeManager* UpgradeManager = GetWorld()->SpawnActor<AUpgradeManager>(UpgradeManagerClass);
  AGlobalDataManager* GlobalDataManager = GetWorld()->SpawnActor<AGlobalDataManager>(GlobalDataManagerClass);
  AGlobalStaticDataManager* GlobalStaticDataManager =
      GetWorld()->SpawnActor<AGlobalStaticDataManager>(GlobalStaticDataManagerClass);
  AStorePhaseManager* StorePhaseManager = GetWorld()->SpawnActor<AStorePhaseManager>(StorePhaseManagerClass);
  ADayManager* DayManager = GetWorld()->SpawnActor<ADayManager>(DayManagerClass);
  AStore* Store = GetWorld()->SpawnActor<AStore>(StoreClass);
  AAbilityManager* AbilityManager = GetWorld()->SpawnActor<AAbilityManager>(AbilityManagerClass);
  ACustomerAIManager* CustomerAIManager = GetWorld()->SpawnActor<ACustomerAIManager>(CustomerAIManagerClass);
  AQuestManager* QuestManager = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass);
  AMarketLevel* MarketLevel = GetWorld()->SpawnActor<AMarketLevel>(MarketLevelClass);
  AMarket* Market = GetWorld()->SpawnActor<AMarket>(MarketClass);
  AMarketEconomy* MarketEconomy = GetWorld()->SpawnActor<AMarketEconomy>(MarketEconomyClass);
  AStatisticsGen* StatisticsGen = GetWorld()->SpawnActor<AStatisticsGen>(StatisticsGenClass);
  ANewsGen* NewsGen = GetWorld()->SpawnActor<ANewsGen>(NewsGenClass);
  AMiniGameManager* MiniGameManager = GetWorld()->SpawnActor<AMiniGameManager>(MiniGameManagerClass);

  PlayerCharacter->DialogueSystem = DialogueSystem;
  PlayerCharacter->NegotiationSystem = NegotiationSystem;
  PlayerCharacter->Market = Market;
  PlayerCharacter->CustomerAIManager = CustomerAIManager;
  PlayerCharacter->NewsGen = NewsGen;
  PlayerCharacter->Store = Store;
  PlayerCharacter->StorePhaseManager = StorePhaseManager;
  PlayerCharacter->DayManager = DayManager;
  PlayerCharacter->LevelManager = LevelManager;
  PlayerCharacter->QuestManager = QuestManager;
  PlayerCharacter->UpgradeManager = UpgradeManager;
  PlayerCharacter->AbilityManager = AbilityManager;
  PlayerCharacter->MiniGameManager = MiniGameManager;

  NegotiationSystem->MarketEconomy = MarketEconomy;
  NegotiationSystem->DialogueSystem = DialogueSystem;
  NegotiationSystem->Store = Store;
  NegotiationSystem->PlayerInventory = PlayerCharacter->PlayerInventoryComponent;
  NegotiationSystem->QuestManager = QuestManager;

  SaveManager->PlayerCharacter = PlayerCharacter;
  SaveManager->SystemsToSave = {
      {"UpgradeManager", UpgradeManager},       {"DayManager", DayManager},     {"Store", Store},
      {"CustomerAIManager", CustomerAIManager}, {"QuestManager", QuestManager}, {"Market", Market},
      {"MarketEconomy", MarketEconomy},         {"NewsGen", NewsGen},

  };
  SaveManager->Market = Market;
  SaveManager->MarketEconomy = MarketEconomy;
  SaveManager->Store = Store;
  SaveManager->UpgradeManager = UpgradeManager;
  SaveManager->GlobalDataManager = GlobalDataManager;
  SaveManager->GlobalStaticDataManager = GlobalStaticDataManager;

  PlayerCommand->PlayerCharacter = PlayerCharacter;

  LevelManager->Store = Store;
  LevelManager->MarketLevel = MarketLevel;

  StorePhaseManager->DayManager = DayManager;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;
  StorePhaseManager->SaveManager = SaveManager;

  DayManager->AbilityManager = AbilityManager;
  DayManager->CustomerAIManager = CustomerAIManager;
  DayManager->MarketEconomy = MarketEconomy;
  DayManager->Market = Market;
  DayManager->MarketLevel = MarketLevel;
  DayManager->StatisticsGen = StatisticsGen;
  DayManager->NewsGen = NewsGen;

  UpgradeManager->CustomerAIManager = CustomerAIManager;
  UpgradeManager->Market = Market;
  UpgradeManager->GlobalDataManager = GlobalDataManager;
  UpgradeManager->GlobalStaticDataManager = GlobalStaticDataManager;
  UpgradeManager->AbilityManager = AbilityManager;

  GlobalDataManager->PlayerCharacter = PlayerCharacter;
  GlobalDataManager->DayManager = DayManager;
  GlobalDataManager->Store = Store;
  GlobalDataManager->QuestManager = QuestManager;
  GlobalDataManager->Market = Market;

  GlobalStaticDataManager->GlobalDataManager = GlobalDataManager;

  MarketLevel->SaveManager = SaveManager;
  MarketLevel->GlobalDataManager = GlobalDataManager;
  MarketLevel->GlobalStaticDataManager = GlobalStaticDataManager;
  MarketLevel->QuestManager = QuestManager;
  MarketLevel->MiniGameManager = MiniGameManager;
  MarketLevel->Market = Market;
  MarketLevel->PlayerCommand = PlayerCommand;

  Store->SaveManager = SaveManager;
  Store->StatisticsGen = StatisticsGen;

  AbilityManager->GlobalDataManager = GlobalDataManager;
  AbilityManager->Market = Market;

  QuestManager->GlobalStaticDataManager = GlobalStaticDataManager;

  Market->GlobalDataManager = GlobalDataManager;
  Market->GlobalStaticDataManager = GlobalStaticDataManager;
  Market->QuestManager = QuestManager;
  Market->MarketEconomy = MarketEconomy;

  CustomerAIManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CustomerAIManager->Market = Market;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;
  CustomerAIManager->QuestManager = QuestManager;

  StatisticsGen->Store = Store;
  StatisticsGen->Market = Market;
  StatisticsGen->MarketEconomy = MarketEconomy;

  NewsGen->GlobalStaticDataManager = GlobalStaticDataManager;

  MiniGameManager->Market = Market;

  UE_LOG(LogTemp, Warning, TEXT("Initializing Game..."));

  LevelManager->LoadLevel(ELevel::Store);

  // ! This runs before level shown callback (blocking = true is bugged).
  // ? Could put in post load callback.
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  if (StorePGGameInstance->bFromSaveGame) SaveManager->LoadCurrentSlotFromDisk();
  // else LevelManager->InitLevel(ELevel::Store);

  StorePhaseManager->Start();

  UE_LOG(LogTemp, Warning, TEXT("Game Initialized."));

  // * Clearing datatable refs mostly.
  CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
}