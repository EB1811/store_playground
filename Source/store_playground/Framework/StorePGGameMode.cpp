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
#include "store_playground/Level/ChurchLevel.h"
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
#include "store_playground/DebtManager/DebtManager.h"
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
#include "store_playground/Tags/TagsComponent.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/Steam/SteamManager.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  UE_LOG(LogTemp, Log, TEXT("Initializing Systems."));

  PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(PlayerCharacter && HUD && LevelManagerClass && GlobalDataManagerClass && GlobalStaticDataManagerClass &&
        UpgradeManagerClass && SaveManagerClass && PlayerCommandClass && StorePhaseManagerClass && DayManagerClass &&
        CustomerAIManagerClass && QuestManagerClass && MarketLevelClass && ChurchLevelClass && MarketClass &&
        MarketEconomyClass && NewsGenClass && StatisticsGenClass && StoreExpansionManagerClass && StoreClass &&
        MiniGameManagerClass && AbilityManagerClass && CutsceneManagerClass && StorePhaseLightingManagerClass &&
        MusicManagerClass && AmbientSoundManagerClass);

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
  CutsceneManager = GetWorld()->SpawnActor<ACutsceneManager>(CutsceneManagerClass);
  AStoreExpansionManager* StoreExpansionManager =
      GetWorld()->SpawnActor<AStoreExpansionManager>(StoreExpansionManagerClass);
  AStore* Store = GetWorld()->SpawnActor<AStore>(StoreClass);
  AAbilityManager* AbilityManager = GetWorld()->SpawnActor<AAbilityManager>(AbilityManagerClass);
  ACustomerAIManager* CustomerAIManager = GetWorld()->SpawnActor<ACustomerAIManager>(CustomerAIManagerClass);
  AQuestManager* QuestManager = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass);
  ATutorialManager* TutorialManager = GetWorld()->SpawnActor<ATutorialManager>(TutorialManagerClass);
  AMarketLevel* MarketLevel = GetWorld()->SpawnActor<AMarketLevel>(MarketLevelClass);
  AChurchLevel* ChurchLevel = GetWorld()->SpawnActor<AChurchLevel>(ChurchLevelClass);
  AMarket* Market = GetWorld()->SpawnActor<AMarket>(MarketClass);
  MarketEconomy = GetWorld()->SpawnActor<AMarketEconomy>(MarketEconomyClass);
  ADebtManager* DebtManager = GetWorld()->SpawnActor<ADebtManager>(DebtManagerClass);
  AStatisticsGen* StatisticsGen = GetWorld()->SpawnActor<AStatisticsGen>(StatisticsGenClass);
  ASteamManager* SteamManager = GetWorld()->SpawnActor<ASteamManager>(SteamManagerClass);
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
  HUD->CustomerAIManager = CustomerAIManager;
  HUD->StatisticsGen = StatisticsGen;
  HUD->SettingsManager = SettingsManager;
  HUD->SaveManager = SaveManager;
  HUD->DebtManager = DebtManager;
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
  NegotiationSystem->SteamManager = SteamManager;

  CutsceneSystem->HUD = HUD;
  CutsceneSystem->DialogueSystem = DialogueSystem;
  CutsceneSystem->CutsceneManager = CutsceneManager;
  CutsceneSystem->SteamManager = SteamManager;

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
      {"DebtManager", DebtManager},
      {"MarketEconomy", MarketEconomy},
      {"StatisticsGen", StatisticsGen},
      {"SteamManager", SteamManager},
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
  SaveManager->ChurchLevel = ChurchLevel;
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
  LevelManager->ChurchLevel = ChurchLevel;

  PlayerCommand->PlayerCharacter = PlayerCharacter;
  PlayerCommand->HUD = HUD;

  StorePhaseManager->PlayerCommand = PlayerCommand;
  StorePhaseManager->SaveManager = SaveManager;
  StorePhaseManager->DayManager = DayManager;
  StorePhaseManager->MusicManager = MusicManager;
  StorePhaseManager->Store = Store;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;
  StorePhaseManager->UpgradeManager = UpgradeManager;

  DayManager->AbilityManager = AbilityManager;
  DayManager->CustomerAIManager = CustomerAIManager;
  DayManager->MarketEconomy = MarketEconomy;
  DayManager->Market = Market;
  DayManager->DebtManager = DebtManager;
  DayManager->MarketLevel = MarketLevel;
  DayManager->StatisticsGen = StatisticsGen;
  DayManager->NewsGen = NewsGen;
  DayManager->SteamManager = SteamManager;
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
      {EUpgradeEffectSystem::DebtManager, DebtManager->Upgradeable},
      {EUpgradeEffectSystem::MarketLevel, MarketLevel->Upgradeable},
      {EUpgradeEffectSystem::StorePhaseManager, StorePhaseManager->Upgradeable},
      {EUpgradeEffectSystem::Store, Store->Upgradeable},
  };

  StoreExpansionManager->PlayerInventoryC = PlayerCharacter->PlayerInventoryComponent;
  StoreExpansionManager->LevelManager = LevelManager;
  StoreExpansionManager->StorePhaseManager = StorePhaseManager;
  StoreExpansionManager->Store = Store;

  StatisticsGen->PlayerInventoryC = PlayerCharacter->PlayerInventoryComponent;
  StatisticsGen->Store = Store;
  StatisticsGen->MarketEconomy = MarketEconomy;
  StatisticsGen->DebtManager = DebtManager;
  StatisticsGen->UpgradeManager = UpgradeManager;
  StatisticsGen->SteamManager = SteamManager;

  SteamManager->GlobalDataManager = GlobalDataManager;

  GlobalDataManager->PlayerCharacter = PlayerCharacter;
  GlobalDataManager->UpgradeManager = UpgradeManager;
  GlobalDataManager->DayManager = DayManager;
  GlobalDataManager->Store = Store;
  GlobalDataManager->QuestManager = QuestManager;
  GlobalDataManager->CutsceneManager = CutsceneManager;
  GlobalDataManager->Market = Market;
  GlobalDataManager->NewsGen = NewsGen;
  GlobalDataManager->StatisticsGen = StatisticsGen;

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

  ChurchLevel->SaveManager = SaveManager;
  ChurchLevel->PlayerCommand = PlayerCommand;

  AbilityManager->GlobalDataManager = GlobalDataManager;
  AbilityManager->GlobalStaticDataManager = GlobalStaticDataManager;
  AbilityManager->Store = Store;
  AbilityManager->Market = Market;

  MiniGameManager->Market = Market;
  MiniGameManager->MarketEconomy = MarketEconomy;
  MiniGameManager->Store = Store;

  QuestManager->GlobalStaticDataManager = GlobalStaticDataManager;
  QuestManager->UpgradeManager = UpgradeManager;

  Store->SaveManager = SaveManager;
  Store->StorePhaseManager = StorePhaseManager;
  Store->CustomerAIManager = CustomerAIManager;
  Store->StatisticsGen = StatisticsGen;
  Store->StorePhaseLightingManager = StorePhaseLightingManager;
  Store->AmbientSoundManager = AmbientSoundManager;

  Market->GlobalDataManager = GlobalDataManager;
  Market->GlobalStaticDataManager = GlobalStaticDataManager;
  Market->QuestManager = QuestManager;
  Market->MarketEconomy = MarketEconomy;

  MarketEconomy->StatisticsGen = StatisticsGen;

  DebtManager->DayManager = DayManager;
  DebtManager->MarketEconomy = MarketEconomy;
  DebtManager->StatisticsGen = StatisticsGen;
  DebtManager->Store = Store;

  CustomerAIManager->PlayerCharacter = PlayerCharacter;
  CustomerAIManager->GlobalStaticDataManager = GlobalStaticDataManager;
  CustomerAIManager->DayManager = DayManager;
  CustomerAIManager->AbilityManager = AbilityManager;
  CustomerAIManager->Market = Market;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;
  CustomerAIManager->QuestManager = QuestManager;

  NewsGen->GlobalStaticDataManager = GlobalStaticDataManager;
  NewsGen->DayManager = DayManager;
  NewsGen->Market = Market;
  NewsGen->StatisticsGen = StatisticsGen;

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

    if (StorePGGameInstance->bFromSaveGame) {
      SaveManager->LoadLevelsAndPlayerFromDisk();

      ASpawnPoint* SpawnPoint =
          *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass).FindByPredicate([](const ASpawnPoint* SpawnPoint) {
            return SpawnPoint->Level == ELevel::Store;
          });
      check(PlayerCharacter);
      check(SpawnPoint);
      PlayerCharacter->SetActorLocation(SpawnPoint->GetActorLocation());
    }

    if (!StorePGGameInstance->bFromSaveGame) MarketEconomy->PerformEconomyTicks(35);
    MarketEconomy->EconomyParams.bRunSimulation = true;

    StorePhaseManager->Start();

    UE_LOG(LogTemp, Log, TEXT("Game Initialized."));

    HUD->SetupInitUIStates();
    HUD->ShowInGameHudWidget();
    HUD->InitGameEndTransition(!StorePGGameInstance->bFromSaveGame);

    // * Clearing datatable refs mostly.
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

    PlayerCharacter->ChangePlayerState(EPlayerState::Normal);
    if (!StorePGGameInstance->bFromSaveGame) NewGameIntro();
  });
}

void AStorePGGameMode::NewGameIntro() {
  if (SettingsManager->GameSettings.bShowTutorials) {
    auto IntroTutorialTag = StringTagsToContainer({FName("Tutorial.GameIntro")});
    PlayerCharacter->PlayerTagsComponent->ConfigurationTags.AppendTags(IntroTutorialTag);
  }

  if (bSkipIntro) return;
  auto CutsceneTags = StringTagsToContainer({FName("Cutscene.GameIntro")});
  auto res = CutsceneManager->PlayPotentialCutscene(CutsceneTags);
}

void AStorePGGameMode::GameOverReset() {
  UE_LOG(LogTemp, Warning, TEXT("Game Over. Resetting game."));

  check(PlayerCharacter);
  PlayerCharacter->GameOverReset();
}
void AStorePGGameMode::ExitToMainMenu() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = false;
  StorePGGameInstance->bFromGame = true;
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

void AStorePGGameMode::DemoGameOver() {
  UE_LOG(LogTemp, Warning, TEXT("Demo Version: Ending game."));

  check(PlayerCharacter);
  PlayerCharacter->DemoGameOver();
}