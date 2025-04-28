// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
#include "PaperZDCharacter.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Level/LevelChangeComponent.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Tags/TagsComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"
#include "Kismet/GameplayStatics.h"

APlayerZDCharacter::APlayerZDCharacter() {
  // Set this character to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  PlayerBehaviourState = EPlayerState::Normal;

  InteractionData.InteractionCheckDistance = 200.0f;

  PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  PlayerTagsComponent = CreateDefaultSubobject<UTagsComponent>(TEXT("TagsComponent"));
}

void APlayerZDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    EnhancedInputComponent->BindAction(InputActions.MoveAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::Move);

    EnhancedInputComponent->BindAction(InputActions.OpenPauseMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenPauseMenu);
    EnhancedInputComponent->BindAction(InputActions.CloseTopOpenMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::CloseTopOpenMenu);
    EnhancedInputComponent->BindAction(InputActions.CloseAllMenusAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::CloseAllMenus);
    EnhancedInputComponent->BindAction(InputActions.OpenInventoryViewAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenInventoryView);
    EnhancedInputComponent->BindAction(InputActions.BuildModeAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::EnterBuildMode);
    EnhancedInputComponent->BindAction(InputActions.OpenNewspaperAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenNewspaper);
    EnhancedInputComponent->BindAction(InputActions.OpenStatisticsAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenStatistics);
    EnhancedInputComponent->BindAction(InputActions.OpenStoreExpansionsAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenStoreExpansions);
    EnhancedInputComponent->BindAction(InputActions.InteractAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::TryInteract);
    EnhancedInputComponent->BindAction(InputActions.AdvanceUIAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::AdvanceUI);
    EnhancedInputComponent->BindAction(InputActions.SkipCutsceneAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::SkipCutscene);
  }
}

void APlayerZDCharacter::BeginPlay() {
  Super::BeginPlay();

  check(SpawnPointClass);

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->AddMappingContext(InputContexts[EPlayerState::Normal], 0);

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  HUD->SetPlayerFocussedFunc = [this]() { ChangePlayerState(EPlayerState::FocussedMenu); };
  HUD->SetPlayerNormalFunc = [this]() { ChangePlayerState(EPlayerState::Normal); };
  HUD->SetPlayerCutsceneFunc = [this]() { ChangePlayerState(EPlayerState::Cutscene); };
  HUD->SetPlayerPausedFunc = [this]() { ChangePlayerState(EPlayerState::Paused); };
}

void APlayerZDCharacter::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void APlayerZDCharacter::ChangePlayerState(EPlayerState NewState) {
  if (PlayerBehaviourState == NewState) return;

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->RemoveMappingContext(InputContexts[PlayerBehaviourState]);
  Subsystem->AddMappingContext(InputContexts[NewState], 0);

  PlayerBehaviourState = NewState;
  UE_LOG(LogTemp, Warning, TEXT("Player state changed to: %s"), *UEnum::GetDisplayValueAsText(NewState).ToString());
}

void APlayerZDCharacter::Move(const FInputActionValue& Value) {
  const FVector2D MovementVector = Value.Get<FVector2D>();
  const FRotator Rotation = GetControlRotation();
  const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
  const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
  const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

  AddMovementInput(ForwardDirection, MovementVector.Y);
  AddMovementInput(RightDirection, MovementVector.X);

  FRotator NewRotation = FQuat::FindBetweenVectors(FVector::ForwardVector, ForwardDirection * MovementVector.Y +
                                                                               RightDirection * MovementVector.X)
                             .Rotator();
  NewRotation.Pitch = 0.0f;
  NewRotation.Roll = 0.0f;
  NewRotation.Normalize();
  SetActorRotation(NewRotation);
}

void APlayerZDCharacter::OpenPauseMenu(const FInputActionValue& Value) {
  check(PlayerBehaviourState != EPlayerState::Cutscene);
  check(SaveManager);

  HUD->OpenPauseMenu(SaveManager);
}

void APlayerZDCharacter::CloseTopOpenMenu(const FInputActionValue& Value) { HUD->CloseTopOpenMenu(); }

void APlayerZDCharacter::CloseAllMenus(const FInputActionValue& Value) { HUD->CloseAllMenus(); }

void APlayerZDCharacter::OpenInventoryView(const FInputActionValue& Value) {
  HUD->SetAndOpenInventoryView(PlayerInventoryComponent, Store);
}

void APlayerZDCharacter::EnterBuildMode(const FInputActionValue& Value) {
  if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning &&
      StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode)
    return;

  StorePhaseManager->BuildMode();
}

void APlayerZDCharacter::OpenNewspaper(const FInputActionValue& Value) {
  HUD->SetAndOpenNewspaper(NewsGen);

  // SaveManager->CreateNewSaveGame();
}

void APlayerZDCharacter::OpenStatistics(const FInputActionValue& Value) { HUD->SetAndOpenStatistics(StatisticsGen); }

void APlayerZDCharacter::OpenStoreExpansions(const FInputActionValue& Value) {
  auto SelectExpansionFunc = [&](EStoreExpansionLevel ExpansionLevel) {
    StoreExpansionManager->SelectExpansion(ExpansionLevel);

    auto LevelReadyFunc = [&]() {
      ASpawnPoint* SpawnPoint =
          *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass).FindByPredicate([](const ASpawnPoint* SpawnPoint) {
            return SpawnPoint->Level == ELevel::Store;
          });
      check(SpawnPoint);

      this->SetActorLocation(SpawnPoint->GetActorLocation());
    };
    LevelManager->ExpandStoreSwitchLevel(LevelReadyFunc);
  };

  HUD->SetAndOpenStoreExpansionsList(StoreExpansionManager, SelectExpansionFunc);
}

void APlayerZDCharacter::TryInteract(const FInputActionValue& Value) {
  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart + GetActorRotation().Vector() * InteractionData.InteractionCheckDistance};

  DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  FHitResult TraceHit;

  if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
    if ((TraceStart - TraceHit.ImpactPoint).Size() <= InteractionData.InteractionCheckDistance)
      if (UInteractionComponent* Interactable = TraceHit.GetActor()->FindComponentByClass<UInteractionComponent>())
        HandleInteraction(Interactable);
}

void APlayerZDCharacter::AdvanceUI(const FInputActionValue& Value) { HUD->AdvanceUI(); }

void APlayerZDCharacter::SkipCutscene(const FInputActionValue& Value) {
  check(PlayerBehaviourState == EPlayerState::Cutscene);

  HUD->SkipCutscene();
}

void APlayerZDCharacter::HandleInteraction(const UInteractionComponent* Interactable) {
  switch (Interactable->InteractionType) {
    case EInteractionType::None: {
      break;
    }
    case EInteractionType::LevelChange: {
      auto LevelChangeC = Interactable->InteractLevelChange();

      EnterNewLevel(LevelChangeC);
      break;
    }
    case EInteractionType::StoreNextPhase: {
      StorePhaseManager->NextPhase();
      break;
    }
    case EInteractionType::UpgradeSelect: {
      auto UpgradeSelectC = Interactable->InteractUpgradeSelect();
      EnterUpgradeSelect(UpgradeSelectC);
      break;
    }
    case EInteractionType::AbilitySelect: {
      EnterAbilitySelect();
      break;
    }
    case EInteractionType::Use: {
      Interactable->InteractUse();
      break;
    }
    case EInteractionType::Buildable: {
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode) break;

      auto Buildable = Interactable->InteractBuildable();
      EnterBuildableDisplay(Buildable.GetValue());
      break;
    }
    case EInteractionType::StockDisplay: {
      // if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning) break;

      auto [DisplayC, DisplayInventoryC] = Interactable->InteractStockDisplay();
      EnterStockDisplay(DisplayC, DisplayInventoryC);
      break;
    }
    case EInteractionType::NPCDialogue: {
      auto DialogueData = Interactable->InteractNPCDialogue();
      EnterDialogue(DialogueData.GetValue());
      break;
    }
    case EInteractionType::WaitingCustomer: {
      auto [CustomerAI, Item] = Interactable->InteractWaitingCustomer();
      EnterNegotiation(CustomerAI, Item);
      break;
    }
    case EInteractionType::UniqueNPCQuest: {
      auto [Dialogue, QuestC, CustomerAI, Item] = Interactable->InteractUniqueNPCQuest();

      EnterQuest(QuestC, Dialogue, CustomerAI, Item);
      break;
    }
    case EInteractionType::NpcStore: {
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning) break;

      auto [NpcStoreC, StoreInventory, Dialogue] = Interactable->InteractNpcStore();
      EnterDialogue(Dialogue->DialogueArray,
                    [this, NpcStoreC, StoreInventory]() { EnterNpcStore(NpcStoreC, StoreInventory); });
      break;
    }
    case EInteractionType::MiniGame: {
      auto [MiniGameC, DialogueC] = Interactable->InteractMiniGame();
      EnterDialogue(DialogueC->DialogueArray, [this, MiniGameC]() { EnterMiniGame(MiniGameC); });
      break;
    }
  }
}

void APlayerZDCharacter::EnterBuildableDisplay(ABuildable* Buildable) {
  auto BuildStockDisplayFunc = [this](ABuildable* Buildable) {
    if (Buildable->BuildingPricesMap[EBuildableType::StockDisplay] > Store->Money) return false;

    Buildable->SetToStockDisplay();
    if (Buildable->BuildableType != EBuildableType::StockDisplay) return false;

    Store->MoneySpent(Buildable->BuildingPricesMap[EBuildableType::StockDisplay]);
    return true;
  };
  auto BuildDecorationFunc = [this](ABuildable* Buildable) {
    if (Buildable->BuildingPricesMap[EBuildableType::Decoration] > Store->Money) return false;

    Buildable->SetToDecoration();
    if (Buildable->BuildableType != EBuildableType::Decoration) return false;

    Store->MoneySpent(Buildable->BuildingPricesMap[EBuildableType::Decoration]);
    return true;
  };

  HUD->SetAndOpenBuildableDisplay(Buildable, BuildStockDisplayFunc, BuildDecorationFunc);
}

void APlayerZDCharacter::EnterStockDisplay(UStockDisplayComponent* StockDisplayC,
                                           UInventoryComponent* DisplayInventoryC) {
  auto PlayerToDisplayFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    // ? Cache the stock display inventories?
    if (TransferItem(SourceInventory, DisplayInventoryC, DroppedItem).bSuccess) Store->InitStockDisplays();
  };
  auto DisplayToPlayerFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == DisplayInventoryC);
    if (TransferItem(SourceInventory, PlayerInventoryComponent, DroppedItem).bSuccess) Store->InitStockDisplays();
  };

  HUD->SetAndOpenStockDisplay(StockDisplayC, DisplayInventoryC, PlayerInventoryComponent, PlayerToDisplayFunc,
                              DisplayToPlayerFunc);
}

void APlayerZDCharacter::EnterUpgradeSelect(UUpgradeSelectComponent* UpgradeSelectC) {
  HUD->SetAndOpenUpgradeSelect(UpgradeSelectC, UpgradeManager);
}

void APlayerZDCharacter::EnterAbilitySelect() { HUD->SetAndOpenAbilitySelect(AbilityManager); }

void APlayerZDCharacter::EnterMiniGame(UMiniGameComponent* MiniGameC) {
  HUD->SetAndOpenMiniGame(MiniGameManager, MiniGameC, Store, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterNpcStore(UNpcStoreComponent* NpcStoreC, UInventoryComponent* StoreInventoryC) {
  // TODO: Add bool on success/failure.
  auto StoreToPlayerFunc = [this, NpcStoreC, StoreInventoryC](UItemBase* DroppedItem,
                                                              UInventoryComponent* SourceInventory) {
    check(SourceInventory == StoreInventoryC);
    Market->BuyItem(NpcStoreC, SourceInventory, PlayerInventoryComponent, Store, DroppedItem, 1);
  };
  auto PlayerToStoreFunc = [this, NpcStoreC, StoreInventoryC](UItemBase* DroppedItem,
                                                              UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    Market->SellItem(NpcStoreC, StoreInventoryC, SourceInventory, Store, DroppedItem, 1);
  };

  HUD->SetAndOpenNPCStore(StoreInventoryC, PlayerInventoryComponent, PlayerToStoreFunc, StoreToPlayerFunc);
}

void APlayerZDCharacter::EnterDialogue(const TArray<FDialogueData> DialogueDataArr,
                                       std::function<void()> OnDialogueEndFunc) {
  if (DialogueDataArr.Num() == 0) {
    if (OnDialogueEndFunc) return OnDialogueEndFunc();
    return;
  }

  DialogueSystem->StartDialogue(DialogueDataArr);
  HUD->SetAndOpenDialogue(DialogueSystem, OnDialogueEndFunc);
}

void APlayerZDCharacter::EnterNegotiation(UCustomerAIComponent* CustomerAI,
                                          UItemBase* Item,
                                          bool bIsQuestAssociated,
                                          UQuestComponent* QuestComponent) {
  NegotiationSystem->StartNegotiation(CustomerAI, Item, CustomerAI->NegotiationAI->StockDisplayInventory,
                                      bIsQuestAssociated, QuestComponent);
  HUD->SetAndOpenNegotiation(NegotiationSystem, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterQuest(UQuestComponent* QuestC,
                                    UDialogueComponent* DialogueC,
                                    UCustomerAIComponent* CustomerAI,
                                    UItemBase* Item) {
  // * Differentiate between when quest is from a customer (store open), and from a npc (e.g., in market).
  if (!CustomerAI)
    return EnterDialogue(DialogueC->DialogueArray, [this, QuestC, CustomerAI]() {
      QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
    });

  switch (QuestC->CustomerAction) {
    case ECustomerAction::PickItem:
    case ECustomerAction::StockCheck:
    case ECustomerAction::SellItem:
      check(CustomerAI->NegotiationAI->RelevantItem);
      if (QuestC->QuestOutcomeType == EQuestOutcomeType::Negotiation)
        EnterDialogue(DialogueC->DialogueArray,
                      [this, Item, CustomerAI, QuestC]() { EnterNegotiation(CustomerAI, Item, true, QuestC); });
      else
        EnterDialogue(DialogueC->DialogueArray, [this, QuestC, CustomerAI, Item]() {
          QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
          EnterNegotiation(CustomerAI, Item);
        });
      break;
    case ECustomerAction::Leave:
    case ECustomerAction::None:
      if (CustomerAI->CustomerState == ECustomerState::Leaving) return;
      EnterDialogue(DialogueC->DialogueArray, [this, QuestC, CustomerAI]() {
        QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
        CustomerAI->CustomerState = ECustomerState::Leaving;
      });
      break;
    default: checkNoEntry(); break;
  }
}

void APlayerZDCharacter::EnterCutscene(const FResolvedCutsceneData ResolvedCutsceneData) {
  CutsceneSystem->StartCutscene(ResolvedCutsceneData);
  HUD->SetAndOpenCutscene(CutsceneSystem);
}

// TODO: Handle cutscene exit and quests.
void APlayerZDCharacter::ExitCurrentAction() {
  if (PlayerBehaviourState == EPlayerState::Normal) return;

  HUD->CloseAllMenus();
  ChangePlayerState(EPlayerState::Normal);
}

void APlayerZDCharacter::EnterNewLevel(ULevelChangeComponent* LevelChangeC) {
  // * Allowed level transitions.
  switch (StorePhaseManager->StorePhaseState) {
    case EStorePhaseState::Morning:
    case EStorePhaseState::MorningBuildMode:
      if (LevelManager->CurrentLevel == ELevel::Store && LevelChangeC->LevelToLoad != ELevel::Market) return;
      if (LevelManager->CurrentLevel == ELevel::Market && LevelChangeC->LevelToLoad != ELevel::Store) return;

      if (StorePhaseManager->StorePhaseState == EStorePhaseState::MorningBuildMode) StorePhaseManager->BuildMode();
      break;
    case EStorePhaseState::ShopOpen: return;
    case EStorePhaseState::Night:
      if (LevelManager->CurrentLevel == ELevel::Store && LevelChangeC->LevelToLoad != ELevel::Church) return;
      if (LevelManager->CurrentLevel == ELevel::Church && LevelChangeC->LevelToLoad != ELevel::Store) return;
      break;
  }

  // ? Put in level manager?
  auto LevelReadyFunc = [this, LevelChangeC]() {
    ASpawnPoint* SpawnPoint = *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)
                                   .FindByPredicate([LevelChangeC](const ASpawnPoint* SpawnPoint) {
                                     return SpawnPoint->Level == LevelChangeC->LevelToLoad;
                                   });
    check(SpawnPoint);

    this->SetActorLocation(SpawnPoint->GetActorLocation());
  };

  LevelManager->BeginLoadLevel(LevelChangeC->LevelToLoad, LevelReadyFunc);
}