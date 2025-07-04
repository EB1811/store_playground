// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Framework/StorePGGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Framework/UtilFuncs.h"
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
#include "store_playground/Lighting/StorePhaseLightingManager.h"
#include "store_playground/Sound/MusicManager.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "store_playground/Cutscene/CutsceneManager.h"
#include "store_playground/UI/SpgHUD.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(PlayerCharacter && HUD && LevelManagerClass && GlobalDataManagerClass && GlobalStaticDataManagerClass &&
        UpgradeManagerClass && SaveManagerClass && PlayerCommandClass && StorePhaseManagerClass && DayManagerClass &&
        CustomerAIManagerClass && QuestManagerClass && MarketLevelClass && MarketClass && MarketEconomyClass &&
        NewsGenClass && StatisticsGenClass && StoreExpansionManagerClass && StoreClass && MiniGameManagerClass &&
        AbilityManagerClass && CutsceneManagerClass && StorePhaseLightingManagerClass);

  // * Initialize the game world and all systems.
  UDialogueSystem* DialogueSystem = NewObject<UDialogueSystem>(this);
  UNegotiationSystem* NegotiationSystem = NewObject<UNegotiationSystem>(this);
  UCutsceneSystem* CutsceneSystem = NewObject<UCutsceneSystem>(this);

  ALevelManager* LevelManager = GetWorld()->SpawnActor<ALevelManager>(LevelManagerClass);
  SaveManager = GetWorld()->SpawnActor<ASaveManager>(SaveManagerClass);
  APlayerCommand* PlayerCommand = GetWorld()->SpawnActor<APlayerCommand>(PlayerCommandClass);
  AUpgradeManager* UpgradeManager = GetWorld()->SpawnActor<AUpgradeManager>(UpgradeManagerClass);
  AGlobalDataManager* GlobalDataManager = GetWorld()->SpawnActor<AGlobalDataManager>(GlobalDataManagerClass);
  AGlobalStaticDataManager* GlobalStaticDataManager =
      GetWorld()->SpawnActor<AGlobalStaticDataManager>(GlobalStaticDataManagerClass);
  StorePhaseManager = GetWorld()->SpawnActor<AStorePhaseManager>(StorePhaseManagerClass);
  ADayManager* DayManager = GetWorld()->SpawnActor<ADayManager>(DayManagerClass);
  AStorePhaseLightingManager* StorePhaseLightingManager =
      GetWorld()->SpawnActor<AStorePhaseLightingManager>(StorePhaseLightingManagerClass);
  AMusicManager* MusicManager = GetWorld()->SpawnActor<AMusicManager>(MusicManagerClass);
  ACutsceneManager* CutsceneManager = GetWorld()->SpawnActor<ACutsceneManager>(CutsceneManagerClass);
  AStoreExpansionManager* StoreExpansionManager =
      GetWorld()->SpawnActor<AStoreExpansionManager>(StoreExpansionManagerClass);
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
  PlayerCharacter->CutsceneSystem = CutsceneSystem;
  PlayerCharacter->Market = Market;
  PlayerCharacter->CustomerAIManager = CustomerAIManager;
  PlayerCharacter->NewsGen = NewsGen;
  PlayerCharacter->StatisticsGen = StatisticsGen;
  PlayerCharacter->SaveManager = SaveManager;
  PlayerCharacter->StoreExpansionManager = StoreExpansionManager;
  PlayerCharacter->Store = Store;
  PlayerCharacter->StorePhaseManager = StorePhaseManager;
  PlayerCharacter->DayManager = DayManager;
  PlayerCharacter->LevelManager = LevelManager;
  PlayerCharacter->QuestManager = QuestManager;
  PlayerCharacter->UpgradeManager = UpgradeManager;
  PlayerCharacter->AbilityManager = AbilityManager;
  PlayerCharacter->MiniGameManager = MiniGameManager;

  HUD->DayManager = DayManager;
  HUD->StorePhaseManager = StorePhaseManager;
  HUD->LevelManager = LevelManager;
  HUD->MarketEconomy = MarketEconomy;
  HUD->Market = Market;
  HUD->StatisticsGen = StatisticsGen;
  HUD->SaveManager = SaveManager;
  HUD->UpgradeManager = UpgradeManager;
  HUD->AbilityManager = AbilityManager;
  HUD->NewsGen = NewsGen;
  HUD->Store = Store;
  HUD->StoreExpansionManager = StoreExpansionManager;

  NegotiationSystem->MarketEconomy = MarketEconomy;
  NegotiationSystem->DialogueSystem = DialogueSystem;
  NegotiationSystem->Store = Store;
  NegotiationSystem->PlayerInventory = PlayerCharacter->PlayerInventoryComponent;
  NegotiationSystem->QuestManager = QuestManager;

  CutsceneSystem->DialogueSystem = DialogueSystem;
  CutsceneSystem->CutsceneManager = CutsceneManager;

  SaveManager->PlayerCharacter = PlayerCharacter;
  SaveManager->SystemsToSave = {
      {"UpgradeManager", UpgradeManager},
      {"DayManager", DayManager},
      {"StoreExpansionManager", StoreExpansionManager},
      {"Store", Store},
      {"AbilityManager", AbilityManager},
      {"CustomerAIManager", CustomerAIManager},
      {"QuestManager", QuestManager},
      {"MarketLevel", MarketLevel},
      {"Market", Market},
      {"MarketEconomy", MarketEconomy},
      {"StatisticsGen", StatisticsGen},
      {"NewsGen", NewsGen},
      {"MiniGameManager", MiniGameManager},
  };
  SaveManager->Market = Market;
  SaveManager->MarketEconomy = MarketEconomy;
  SaveManager->Store = Store;
  SaveManager->UpgradeManager = UpgradeManager;
  SaveManager->GlobalDataManager = GlobalDataManager;
  SaveManager->GlobalStaticDataManager = GlobalStaticDataManager;
  SaveManager->DayManager = DayManager;

  PlayerCommand->PlayerCharacter = PlayerCharacter;

  LevelManager->PlayerTags = PlayerCharacter->PlayerTagsComponent;
  LevelManager->DayManager = DayManager;
  LevelManager->StoreExpansionManager = StoreExpansionManager;
  LevelManager->CutsceneManager = CutsceneManager;
  LevelManager->Store = Store;
  LevelManager->MarketLevel = MarketLevel;

  StorePhaseManager->PlayerCommand = PlayerCommand;
  StorePhaseManager->SaveManager = SaveManager;
  StorePhaseManager->DayManager = DayManager;
  StorePhaseManager->StorePhaseLightingManager = StorePhaseLightingManager;
  StorePhaseManager->MusicManager = MusicManager;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;

  DayManager->AbilityManager = AbilityManager;
  DayManager->CustomerAIManager = CustomerAIManager;
  DayManager->MarketEconomy = MarketEconomy;
  DayManager->Market = Market;
  DayManager->MarketLevel = MarketLevel;
  DayManager->StatisticsGen = StatisticsGen;
  DayManager->NewsGen = NewsGen;
  DayManager->Store = Store;

  UpgradeManager->CustomerAIManager = CustomerAIManager;
  UpgradeManager->Market = Market;
  UpgradeManager->GlobalDataManager = GlobalDataManager;
  UpgradeManager->GlobalStaticDataManager = GlobalStaticDataManager;
  UpgradeManager->AbilityManager = AbilityManager;
  UpgradeManager->StatisticsGen = StatisticsGen;

  GlobalDataManager->PlayerCharacter = PlayerCharacter;
  GlobalDataManager->DayManager = DayManager;
  GlobalDataManager->Store = Store;
  GlobalDataManager->QuestManager = QuestManager;
  GlobalDataManager->Market = Market;

  GlobalStaticDataManager->GlobalDataManager = GlobalDataManager;

  CutsceneManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CutsceneManager->PlayerCommand = PlayerCommand;

  MarketLevel->SaveManager = SaveManager;
  MarketLevel->GlobalDataManager = GlobalDataManager;
  MarketLevel->GlobalStaticDataManager = GlobalStaticDataManager;
  MarketLevel->QuestManager = QuestManager;
  MarketLevel->MiniGameManager = MiniGameManager;
  MarketLevel->Market = Market;
  MarketLevel->MarketEconomy = MarketEconomy;
  MarketLevel->PlayerCommand = PlayerCommand;

  StoreExpansionManager->Store = Store;

  AbilityManager->GlobalDataManager = GlobalDataManager;
  AbilityManager->Store = Store;
  AbilityManager->Market = Market;

  QuestManager->GlobalStaticDataManager = GlobalStaticDataManager;

  Store->SaveManager = SaveManager;
  Store->StatisticsGen = StatisticsGen;

  Market->GlobalDataManager = GlobalDataManager;
  Market->GlobalStaticDataManager = GlobalStaticDataManager;
  Market->QuestManager = QuestManager;
  Market->MarketEconomy = MarketEconomy;

  MarketEconomy->StatisticsGen = StatisticsGen;

  CustomerAIManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CustomerAIManager->Market = Market;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;
  CustomerAIManager->QuestManager = QuestManager;

  StatisticsGen->PlayerInventoryC = PlayerCharacter->PlayerInventoryComponent;
  StatisticsGen->Store = Store;
  StatisticsGen->MarketEconomy = MarketEconomy;

  NewsGen->GlobalStaticDataManager = GlobalStaticDataManager;
  NewsGen->Market = Market;

  MiniGameManager->Market = Market;

  // todo-low: Put loading screen here.
  UE_LOG(LogTemp, Warning, TEXT("Initializing Game..."));

  // * Load systems save data, then store level, then load level save data.
  // No way to do a blocking load so need a callback.
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);

  SaveManager->LoadSaveGameSlots();
  if (StorePGGameInstance->bFromSaveGame) SaveManager->LoadSystemsFromDisk(StorePGGameInstance->SaveSlotIndex);

  LevelManager->InitLoadStore([this, StorePGGameInstance]() {
    // ! Enter level post transition blueprint function is not called.
    if (StorePGGameInstance->bFromSaveGame) SaveManager->LoadLevelsAndPlayerFromDisk();

    StorePhaseManager->Start();

    HUD->SetupInitUIStates();
    HUD->ShowInGameHudWidget();

    APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
    ASpawnPoint* SpawnPoint =
        *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass).FindByPredicate([](const ASpawnPoint* SpawnPoint) {
          return SpawnPoint->Level == ELevel::Store;
        });
    check(PlayerCharacter);
    check(SpawnPoint);

    PlayerCharacter->SetActorLocation(SpawnPoint->GetActorLocation());

    UE_LOG(LogTemp, Warning, TEXT("Game Initialized."));

    // * Clearing datatable refs mostly.
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
  });
}

void AStorePGGameMode::GameOverReset() {
  UE_LOG(LogTemp, Warning, TEXT("Game Over. Resetting game."));

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = false;
  StorePGGameInstance->bFromGameOver = true;

  UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true);
}

// * Reopen the game with the selected save slot.
void AStorePGGameMode::LoadGame(int32 SlotIndex) {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SlotIndex;

  StorePGGameInstance->bFromGameOver = false;

  UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true);
}