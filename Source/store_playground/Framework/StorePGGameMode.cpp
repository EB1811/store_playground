// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Framework/StorePGGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/Framework/SettingsManager.h"
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
#include "store_playground/Sound/AmbientSoundManager.h"
#include "store_playground/Sound/MusicManager.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "store_playground/Tutorial/TutorialManager.h"
#include "store_playground/Cutscene/CutsceneManager.h"
#include "store_playground/UI/SpgHUD.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  UE_LOG(LogTemp, Log, TEXT("Initializing Systems."));

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(PlayerCharacter && HUD && LevelManagerClass && GlobalDataManagerClass && GlobalStaticDataManagerClass &&
        UpgradeManagerClass && SaveManagerClass && PlayerCommandClass && StorePhaseManagerClass && DayManagerClass &&
        CustomerAIManagerClass && QuestManagerClass && MarketLevelClass && MarketClass && MarketEconomyClass &&
        NewsGenClass && StatisticsGenClass && StoreExpansionManagerClass && StoreClass && MiniGameManagerClass &&
        AbilityManagerClass && CutsceneManagerClass && StorePhaseLightingManagerClass && MusicManagerClass &&
        AmbientSoundManagerClass);

  UDialogueSystem* DialogueSystem = NewObject<UDialogueSystem>(this);
  UNegotiationSystem* NegotiationSystem = NewObject<UNegotiationSystem>(this);
  UCutsceneSystem* CutsceneSystem = NewObject<UCutsceneSystem>(this);

  SettingsManager = GetWorld()->SpawnActor<ASettingsManager>(SettingsManagerClass);
  SaveManager = GetWorld()->SpawnActor<ASaveManager>(SaveManagerClass);
  ALevelManager* LevelManager = GetWorld()->SpawnActor<ALevelManager>(LevelManagerClass);
  AUpgradeManager* UpgradeManager = GetWorld()->SpawnActor<AUpgradeManager>(UpgradeManagerClass);
  APlayerCommand* PlayerCommand = GetWorld()->SpawnActor<APlayerCommand>(PlayerCommandClass);
  AGlobalDataManager* GlobalDataManager = GetWorld()->SpawnActor<AGlobalDataManager>(GlobalDataManagerClass);
  AGlobalStaticDataManager* GlobalStaticDataManager =
      GetWorld()->SpawnActor<AGlobalStaticDataManager>(GlobalStaticDataManagerClass);
  StorePhaseManager = GetWorld()->SpawnActor<AStorePhaseManager>(StorePhaseManagerClass);
  ADayManager* DayManager = GetWorld()->SpawnActor<ADayManager>(DayManagerClass);
  AStorePhaseLightingManager* StorePhaseLightingManager =
      GetWorld()->SpawnActor<AStorePhaseLightingManager>(StorePhaseLightingManagerClass);
  AMusicManager* MusicManager = GetWorld()->SpawnActor<AMusicManager>(MusicManagerClass);
  AAmbientSoundManager* AmbientSoundManager = GetWorld()->SpawnActor<AAmbientSoundManager>(AmbientSoundManagerClass);
  ACutsceneManager* CutsceneManager = GetWorld()->SpawnActor<ACutsceneManager>(CutsceneManagerClass);
  AStoreExpansionManager* StoreExpansionManager =
      GetWorld()->SpawnActor<AStoreExpansionManager>(StoreExpansionManagerClass);
  AStore* Store = GetWorld()->SpawnActor<AStore>(StoreClass);
  AAbilityManager* AbilityManager = GetWorld()->SpawnActor<AAbilityManager>(AbilityManagerClass);
  ACustomerAIManager* CustomerAIManager = GetWorld()->SpawnActor<ACustomerAIManager>(CustomerAIManagerClass);
  AQuestManager* QuestManager = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass);
  ATutorialManager* TutorialManager = GetWorld()->SpawnActor<ATutorialManager>(TutorialManagerClass);
  AMarketLevel* MarketLevel = GetWorld()->SpawnActor<AMarketLevel>(MarketLevelClass);
  AMarket* Market = GetWorld()->SpawnActor<AMarket>(MarketClass);
  MarketEconomy = GetWorld()->SpawnActor<AMarketEconomy>(MarketEconomyClass);
  AStatisticsGen* StatisticsGen = GetWorld()->SpawnActor<AStatisticsGen>(StatisticsGenClass);
  ANewsGen* NewsGen = GetWorld()->SpawnActor<ANewsGen>(NewsGenClass);
  AMiniGameManager* MiniGameManager = GetWorld()->SpawnActor<AMiniGameManager>(MiniGameManagerClass);

  PlayerCharacter->DialogueSystem = DialogueSystem;
  PlayerCharacter->NegotiationSystem = NegotiationSystem;
  PlayerCharacter->CutsceneSystem = CutsceneSystem;
  PlayerCharacter->Market = Market;
  PlayerCharacter->StoreExpansionManager = StoreExpansionManager;
  PlayerCharacter->Store = Store;
  PlayerCharacter->StorePhaseManager = StorePhaseManager;
  PlayerCharacter->LevelManager = LevelManager;
  PlayerCharacter->QuestManager = QuestManager;
  PlayerCharacter->TutorialManager = TutorialManager;
  PlayerCharacter->MiniGameManager = MiniGameManager;

  HUD->DayManager = DayManager;
  HUD->StorePhaseManager = StorePhaseManager;
  HUD->LevelManager = LevelManager;
  HUD->MarketEconomy = MarketEconomy;
  HUD->Market = Market;
  HUD->StatisticsGen = StatisticsGen;
  HUD->SettingsManager = SettingsManager;
  HUD->SaveManager = SaveManager;
  HUD->UpgradeManager = UpgradeManager;
  HUD->AbilityManager = AbilityManager;
  HUD->NewsGen = NewsGen;
  HUD->Store = Store;
  HUD->StoreExpansionManager = StoreExpansionManager;

  NegotiationSystem->MarketEconomy = MarketEconomy;
  NegotiationSystem->CustomerAIManager = CustomerAIManager;
  NegotiationSystem->DialogueSystem = DialogueSystem;
  NegotiationSystem->Store = Store;
  NegotiationSystem->PlayerInventory = PlayerCharacter->PlayerInventoryComponent;
  NegotiationSystem->QuestManager = QuestManager;

  CutsceneSystem->HUD = HUD;
  CutsceneSystem->DialogueSystem = DialogueSystem;
  CutsceneSystem->CutsceneManager = CutsceneManager;

  SettingsManager->SaveManager = SaveManager;
  SettingsManager->DayManager = DayManager;

  SaveManager->PlayerCharacter = PlayerCharacter;
  SaveManager->SystemsToSave = {
      {"UpgradeManager", UpgradeManager},
      {"StorePhaseManager", StorePhaseManager},
      {"DayManager", DayManager},
      {"StoreExpansionManager", StoreExpansionManager},
      {"Store", Store},
      {"AbilityManager", AbilityManager},
      {"CustomerAIManager", CustomerAIManager},
      {"QuestManager", QuestManager},
      {"CutsceneManager", CutsceneManager},
      {"TutorialManager", TutorialManager},
      {"MarketLevel", MarketLevel},
      {"Market", Market},
      {"MarketEconomy", MarketEconomy},
      {"StatisticsGen", StatisticsGen},
      {"NewsGen", NewsGen},
      {"MiniGameManager", MiniGameManager},
  };
  SaveManager->LevelManager = LevelManager;
  SaveManager->Market = Market;
  SaveManager->UpgradeManager = UpgradeManager;
  SaveManager->GlobalDataManager = GlobalDataManager;
  SaveManager->GlobalStaticDataManager = GlobalStaticDataManager;
  SaveManager->DayManager = DayManager;
  SaveManager->StorePhaseManager = StorePhaseManager;
  SaveManager->MarketEconomy = MarketEconomy;
  SaveManager->MarketLevel = MarketLevel;
  SaveManager->Store = Store;

  LevelManager->PlayerTags = PlayerCharacter->PlayerTagsComponent;
  LevelManager->StorePhaseManager = StorePhaseManager;
  LevelManager->DayManager = DayManager;
  LevelManager->StoreExpansionManager = StoreExpansionManager;
  LevelManager->PlayerCommand = PlayerCommand;
  LevelManager->StorePhaseLightingManager = StorePhaseLightingManager;
  LevelManager->CutsceneManager = CutsceneManager;
  LevelManager->Store = Store;
  LevelManager->MarketLevel = MarketLevel;

  PlayerCommand->PlayerCharacter = PlayerCharacter;
  PlayerCommand->HUD = HUD;

  StorePhaseManager->PlayerCommand = PlayerCommand;
  StorePhaseManager->SaveManager = SaveManager;
  StorePhaseManager->DayManager = DayManager;
  StorePhaseManager->MusicManager = MusicManager;
  StorePhaseManager->Store = Store;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;

  DayManager->AbilityManager = AbilityManager;
  DayManager->CustomerAIManager = CustomerAIManager;
  DayManager->MarketEconomy = MarketEconomy;
  DayManager->Market = Market;
  DayManager->MarketLevel = MarketLevel;
  DayManager->StatisticsGen = StatisticsGen;
  DayManager->NewsGen = NewsGen;
  DayManager->Store = Store;

  UpgradeManager->StatisticsGen = StatisticsGen;
  UpgradeManager->HUD = HUD;
  UpgradeManager->GlobalDataManager = GlobalDataManager;
  UpgradeManager->GlobalStaticDataManager = GlobalStaticDataManager;
  UpgradeManager->UpgradeableMap = {
      {EUpgradeEffectSystem::Ability, AbilityManager->Upgradeable},
      {EUpgradeEffectSystem::CustomerAI, CustomerAIManager->Upgradeable},
      {EUpgradeEffectSystem::GlobalData, GlobalDataManager->Upgradeable},
      {EUpgradeEffectSystem::Market, Market->Upgradeable},
      {EUpgradeEffectSystem::MarketEconomy, MarketEconomy->Upgradeable},
      {EUpgradeEffectSystem::MarketLevel, MarketLevel->Upgradeable},
      {EUpgradeEffectSystem::StorePhaseManager, StorePhaseManager->Upgradeable},
  };

  StoreExpansionManager->LevelManager = LevelManager;
  StoreExpansionManager->Store = Store;

  StatisticsGen->PlayerInventoryC = PlayerCharacter->PlayerInventoryComponent;
  StatisticsGen->Store = Store;
  StatisticsGen->MarketEconomy = MarketEconomy;
  StatisticsGen->UpgradeManager = UpgradeManager;

  GlobalDataManager->PlayerCharacter = PlayerCharacter;
  GlobalDataManager->UpgradeManager = UpgradeManager;
  GlobalDataManager->DayManager = DayManager;
  GlobalDataManager->Store = Store;
  GlobalDataManager->QuestManager = QuestManager;
  GlobalDataManager->Market = Market;
  GlobalDataManager->NewsGen = NewsGen;

  GlobalStaticDataManager->GlobalDataManager = GlobalDataManager;

  CutsceneManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CutsceneManager->PlayerTags = PlayerCharacter->PlayerTagsComponent;
  CutsceneManager->PlayerCommand = PlayerCommand;

  TutorialManager->SettingsManager = SettingsManager;
  TutorialManager->PlayerTags = PlayerCharacter->PlayerTagsComponent;
  TutorialManager->PlayerCommand = PlayerCommand;

  MarketLevel->SaveManager = SaveManager;
  MarketLevel->GlobalDataManager = GlobalDataManager;
  MarketLevel->GlobalStaticDataManager = GlobalStaticDataManager;
  MarketLevel->StatisticsGen = StatisticsGen;
  MarketLevel->QuestManager = QuestManager;
  MarketLevel->MiniGameManager = MiniGameManager;
  MarketLevel->Market = Market;
  MarketLevel->MarketEconomy = MarketEconomy;
  MarketLevel->PlayerCommand = PlayerCommand;

  AbilityManager->GlobalDataManager = GlobalDataManager;
  AbilityManager->GlobalStaticDataManager = GlobalStaticDataManager;
  AbilityManager->Store = Store;
  AbilityManager->Market = Market;

  MiniGameManager->Market = Market;
  MiniGameManager->MarketEconomy = MarketEconomy;
  MiniGameManager->Store = Store;

  QuestManager->GlobalStaticDataManager = GlobalStaticDataManager;

  Store->SaveManager = SaveManager;
  Store->StorePhaseManager = StorePhaseManager;
  Store->StatisticsGen = StatisticsGen;
  Store->StorePhaseLightingManager = StorePhaseLightingManager;
  Store->AmbientSoundManager = AmbientSoundManager;

  Market->GlobalDataManager = GlobalDataManager;
  Market->GlobalStaticDataManager = GlobalStaticDataManager;
  Market->QuestManager = QuestManager;
  Market->MarketEconomy = MarketEconomy;

  MarketEconomy->StatisticsGen = StatisticsGen;

  CustomerAIManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CustomerAIManager->AbilityManager = AbilityManager;
  CustomerAIManager->Market = Market;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;
  CustomerAIManager->QuestManager = QuestManager;

  NewsGen->GlobalStaticDataManager = GlobalStaticDataManager;
  NewsGen->Market = Market;

  UE_LOG(LogTemp, Log, TEXT("Initializing Game..."));

  // * Settings and save data.
  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  check(Subsystem);
  UEnhancedInputUserSettings* EISettings = Subsystem->GetUserSettings();
  check(EISettings);
  for (const auto& StateContext : PlayerCharacter->InputContexts)
    EISettings->RegisterInputMappingContext(StateContext.Value);
  SettingsManager->LoadSettings();
  SaveManager->LoadSaveGameSlots();

  // * Load systems save data, then store level, then load level save data.
  // No way to do a blocking load so need a callback.
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  if (StorePGGameInstance->bFromSaveGame) SaveManager->LoadSystemsFromDisk(StorePGGameInstance->SaveSlotIndex);

  LevelManager->InitLoadStore([this, StorePGGameInstance]() {
    // ! Enter level post transition blueprint function is not called.
    if (StorePGGameInstance->bFromSaveGame) SaveManager->LoadLevelsAndPlayerFromDisk();

    if (!StorePGGameInstance->bFromSaveGame) MarketEconomy->PerformEconomyTicks(35);
    MarketEconomy->EconomyParams.bRunSimulation = true;

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

    UE_LOG(LogTemp, Log, TEXT("Game Initialized."));
    HUD->InitGameEndTransition();

    // * Clearing datatable refs mostly.
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
  });
}

void AStorePGGameMode::GameOverReset() {
  UE_LOG(LogTemp, Warning, TEXT("Game Over. Resetting game."));

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  check(PlayerCharacter);
  PlayerCharacter->GameOverReset();
}
void AStorePGGameMode::ExitToMainMenu() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = false;
  StorePGGameInstance->bFromGameOver = false;

  HUD->StartGameLoadTransition([this]() { UGameplayStatics::OpenLevel(GetWorld(), "MainMenuMap", true); });
}
// * Reopen the game with the selected save slot.
void AStorePGGameMode::LoadGame(int32 SlotIndex) {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SlotIndex;

  StorePGGameInstance->bFromGameOver = false;

  HUD->StartGameLoadTransition([this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); });
}