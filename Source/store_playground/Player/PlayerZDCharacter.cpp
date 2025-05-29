// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
#include "Engine/StaticMesh.h"
#include "Internationalization/Text.h"
#include "Materials/MaterialInstance.h"
#include "Misc/AssertionMacros.h"
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
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"

APlayerZDCharacter::APlayerZDCharacter() {
  // Set this character to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  PlayerBehaviourState = EPlayerState::Normal;

  OcclusionCheckData.LastOcclusionCheckTime = 0.0f;
  OcclusionCheckData.OcclusionCheckFrequency = 200.0f;

  InteractionData.InteractionCheckFrequency = 0.0f;
  InteractionData.InteractionCheckDistance = 200.0f;
  InteractionData.InteractionCheckRadius = 50.0f;

  PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  PlayerTagsComponent = CreateDefaultSubobject<UTagsComponent>(TEXT("TagsComponent"));
  PlayerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerWidgetComponent"));
}

void APlayerZDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    EnhancedInputComponent->BindAction(InputActions.MoveAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::Move);

    EnhancedInputComponent->BindAction(InputActions.OpenPauseMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenPauseMenu);
    EnhancedInputComponent->BindAction(InputActions.CloseTopOpenMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::PlayerCloseTopOpenMenu);
    EnhancedInputComponent->BindAction(InputActions.CloseAllMenusAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::PlayerCloseAllMenus);
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
                                       &APlayerZDCharacter::Interact);
    EnhancedInputComponent->BindAction(InputActions.AdvanceUIAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::AdvanceUI);
    EnhancedInputComponent->BindAction(InputActions.SkipCutsceneAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::SkipCutscene);
  }
}

void APlayerZDCharacter::BeginPlay() {
  Super::BeginPlay();

  // PlayerWidgetComponent->SetVisibility(true);
  // PlayerWidgetComponent->SetWorldLocation(GetPawnViewLocation());
  // PlayerWidgetComponent->SetWorldRotation(FRotator(45, 90, 0));  // y, z, x

  check(SpawnPointClass);

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->AddMappingContext(InputContexts[EPlayerState::Normal], 0);

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  HUD->SetPlayerFocussedFunc = [this]() { ChangePlayerState(EPlayerState::FocussedMenu); };
  HUD->SetPlayerNormalFunc = [this]() { ChangePlayerState(EPlayerState::Normal); };
  HUD->SetPlayerCutsceneFunc = [this]() { ChangePlayerState(EPlayerState::Cutscene); };
  HUD->SetPlayerPausedFunc = [this]() { ChangePlayerState(EPlayerState::Paused); };

  HUD->PlayerInputActions = InputActions;
  HUD->ShowInGameHudWidget();

  CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

void APlayerZDCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (PlayerBehaviourState == EPlayerState::Normal &&
      GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionData.InteractionCheckFrequency)
    CheckForInteraction();

  if (PlayerBehaviourState == EPlayerState::Normal &&
      GetWorld()->TimeSince(OcclusionCheckData.LastOcclusionCheckTime) > OcclusionCheckData.OcclusionCheckFrequency) {
    HandleOcclusion();
  }
}

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

  FacingDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
  FacingDirection.Normalize();

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

  UGameplayStatics::SetGamePaused(GetWorld(), PlayerBehaviourState == EPlayerState::Paused);
}

void APlayerZDCharacter::PlayerCloseTopOpenMenu(const FInputActionValue& Value) { HUD->PlayerCloseTopOpenMenu(); }

void APlayerZDCharacter::PlayerCloseAllMenus(const FInputActionValue& Value) { HUD->PlayerCloseAllMenus(); }

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
    if (!StoreExpansionManager->SelectExpansion(ExpansionLevel)) return;

    auto LevelReadyFunc = [&]() {
      ASpawnPoint* SpawnPoint =
          *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass).FindByPredicate([](const ASpawnPoint* SpawnPoint) {
            return SpawnPoint->Level == ELevel::Store;
          });
      check(SpawnPoint);

      this->SetActorLocation(SpawnPoint->GetActorLocation());
      UE_LOG(LogTemp, Warning, TEXT("Player location set to spawn point: %s"), *SpawnPoint->GetName());
    };
    LevelManager->ExpandStoreSwitchLevel(LevelReadyFunc);
  };

  HUD->SetAndOpenStoreExpansionsList(StoreExpansionManager, SelectExpansionFunc);
}

// Rechecking on input to avoid problems with the interaction frequency not keeping up.
void APlayerZDCharacter::Interact(const FInputActionValue& Value) {
  if (CheckForInteraction()) HandleInteraction(CurrentInteractableC);
}

void APlayerZDCharacter::AdvanceUI(const FInputActionValue& Value) { HUD->AdvanceUI(); }

void APlayerZDCharacter::SkipCutscene(const FInputActionValue& Value) {
  check(PlayerBehaviourState == EPlayerState::Cutscene);

  HUD->SkipCutscene();
}

void APlayerZDCharacter::HandleOcclusion() {
  OcclusionCheckData.LastOcclusionCheckTime = GetWorld()->GetTimeSeconds();

  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{CameraManager->GetCameraLocation()};
  FCollisionShape Sphere = FCollisionShape::MakeSphere(25.0f);

  // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  TraceParams.AddIgnoredActor(CameraManager->GetViewTarget());
  FHitResult TraceHit;

  if (GetWorld()->SweepSingleByChannel(TraceHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere,
                                       TraceParams)) {
    if (auto* HitActor = TraceHit.GetActor()) {
      if (!HitActor->ActorHasTag(OcclusionCheckData.OcclusionTag)) return;

      if (OccludedActors.Contains(HitActor)) return;
      OccludedActors.AddUnique(HitActor);

      // HitActor->SetActorHiddenInGame(true);
      TArray<UStaticMeshComponent*> Components;
      HitActor->GetComponents<UStaticMeshComponent>(Components);
      for (auto* StaticMeshComponent : Components) {
        StaticMeshComponent->bCastHiddenShadow = true;
        StaticMeshComponent->bAffectIndirectLightingWhileHidden = true;
        StaticMeshComponent->SetHiddenInGame(true);
      }

      // ! Hiding for now.
      // * Need a way to save each of the original materials.
      // OccludedActors.AddUnique(HitActor);
      // TArray<UStaticMeshComponent*> Components;
      // HitActor->GetComponents<UStaticMeshComponent>(Components);
      // for (auto* StaticMeshComponent : Components)
      //   for (int i = 0; i < StaticMeshComponent->GetStaticMesh()->GetStaticMaterials().Num(); i++)
      //     StaticMeshComponent->SetMaterial(i, OcclusionCheckData.OcclusionMaterial);
    }
  } else {
    for (auto* OccludedActor : OccludedActors)
      if (OccludedActor) {
        TArray<UStaticMeshComponent*> Components;
        OccludedActor->GetComponents<UStaticMeshComponent>(Components);
        for (auto* StaticMeshComponent : Components) StaticMeshComponent->SetHiddenInGame(false);
      }
    OccludedActors.Empty();
  }
}

auto APlayerZDCharacter::CheckForInteraction() -> bool {
  InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart + FacingDirection * InteractionData.InteractionCheckDistance};
  FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionData.InteractionCheckRadius);

  // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  FHitResult TraceHit;

  if (GetWorld()->SweepSingleByChannel(TraceHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere,
                                       TraceParams)) {
    if ((TraceStart - TraceHit.ImpactPoint).Size() <= InteractionData.InteractionCheckDistance)
      if (UInteractionComponent* Interactable = TraceHit.GetActor()->FindComponentByClass<UInteractionComponent>()) {
        if (!IsInteractable(Interactable)) return false;

        CurrentInteractableC = Interactable;
        HUD->OpenInteractionPopup(FText::FromName("!"));
        return true;
      }
  }

  if (CurrentInteractableC) {
    CurrentInteractableC = nullptr;
    HUD->CloseInteractionPopup();
  }

  return false;
}

auto APlayerZDCharacter::IsInteractable(const UInteractionComponent* Interactable) const -> bool {
  switch (Interactable->InteractionType) {
    case EInteractionType::None: return false; break;
    case EInteractionType::Buildable:
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode) return false;
      break;
    case EInteractionType::StockDisplay:
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning) return false;
      break;
  }

  return true;
}

void APlayerZDCharacter::HandleInteraction(UInteractionComponent* Interactable) {
  HUD->CloseInteractionPopup();

  // HUD->EarlyCloseWidgetFunc = [this, Interactable]() {
  //   check(Interactable);
  //   Interactable->EndInteraction();
  // };

  switch (Interactable->InteractionType) {
    case EInteractionType::LevelChange: {
      auto LevelChangeC = Interactable->InteractLevelChange();

      EnterNewLevel(LevelChangeC);
      break;
    }
    case EInteractionType::LeaveStore: {
      LeaveStore();
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
      auto Buildable = Interactable->InteractBuildable();
      EnterBuildableDisplay(Buildable.GetValue());
      break;
    }
    case EInteractionType::StockDisplay: {
      auto [DisplayC, DisplayInventoryC] = Interactable->InteractStockDisplay();
      EnterStockDisplay(DisplayC, DisplayInventoryC);
      break;
    }
    case EInteractionType::NPCDialogue: {
      auto [DialogueC, SpriteAnimC] = Interactable->InteractNPCDialogue();

      // ? Move to function?
      SetupNpcInteraction(SpriteAnimC);
      EnterDialogue(DialogueC, [this, SpriteAnimC]() { SpriteAnimC->ReturnToOgRotation(); });
      break;
    }
    case EInteractionType::Customer: {
      auto [DialogueC, CustomerAI, SpriteAnimC] = Interactable->InteractCustomer();

      // ? Move to function?
      CustomerAI->CustomerState = ECustomerState::BrowsingTalking;
      SetupCustomerInteraction(CustomerAI, SpriteAnimC);
      EnterDialogue(DialogueC, [this, CustomerAI, SpriteAnimC]() {
        CustomerAI->CustomerState = ECustomerState::Browsing;
        SpriteAnimC->ReturnToOgRotation();
      });
      break;
    }
    case EInteractionType::WaitingCustomer: {
      auto [CustomerAI, Item, SpriteAnimC] = Interactable->InteractWaitingCustomer();

      SetupCustomerInteraction(CustomerAI, SpriteAnimC);
      EnterNegotiation(CustomerAI, Item);
      break;
    }
    case EInteractionType::UniqueNPCQuest: {
      auto [Dialogue, QuestC, CustomerAI, Item, SpriteAnimC] = Interactable->InteractUniqueNPCQuest();

      SetupNpcInteraction(SpriteAnimC);
      EnterQuest(QuestC, Dialogue, SpriteAnimC, CustomerAI, Item);
      break;
    }
    case EInteractionType::NpcStore: {
      auto [NpcStoreC, StoreInventory, DialogueC] = Interactable->InteractNpcStore();
      EnterDialogue(DialogueC, [this, NpcStoreC, StoreInventory]() { EnterNpcStore(NpcStoreC, StoreInventory); });
      break;
    }
    case EInteractionType::MiniGame: {
      auto [MiniGameC, DialogueC] = Interactable->InteractMiniGame();
      EnterDialogue(DialogueC, [this, MiniGameC]() { EnterMiniGame(MiniGameC); });
      break;
    }
    default: checkNoEntry();
  }
}

void APlayerZDCharacter::SetupNpcInteraction(USimpleSpriteAnimComponent* SpriteAnimC) {
  SpriteAnimC->TurnToPlayer(GetActorLocation());

  // ? Move to player?
  HUD->EarlyCloseWidgetFunc = [this, SpriteAnimC]() {
    check(SpriteAnimC);
    SpriteAnimC->ReturnToOgRotation();
  };
}
void APlayerZDCharacter::SetupCustomerInteraction(UCustomerAIComponent* CustomerAI,
                                                  USimpleSpriteAnimComponent* SpriteAnimC) {
  SpriteAnimC->TurnToPlayer(GetActorLocation());

  // ? Move to player?
  switch (CustomerAI->CustomerState) {
    case ECustomerState::BrowsingTalking:
      HUD->EarlyCloseWidgetFunc = [this, CustomerAI, SpriteAnimC]() {
        check(CustomerAI);
        check(SpriteAnimC);
        CustomerAI->CustomerState = ECustomerState::Browsing;
        SpriteAnimC->ReturnToOgRotation();
      };
      break;
    case ECustomerState::Requesting:
      HUD->EarlyCloseWidgetFunc = [this, CustomerAI, SpriteAnimC]() {
        check(CustomerAI);
        check(SpriteAnimC);
        CustomerAI->LeaveRequestDialogue();
        SpriteAnimC->ReturnToOgRotation();
      };
      break;
    default: break;
  }
}

void APlayerZDCharacter::EnterBuildableDisplay(ABuildable* Buildable) {
  auto BuildStockDisplayFunc = [this](ABuildable* Buildable) { return Store->BuildStockDisplay(Buildable); };
  auto BuildDecorationFunc = [this](ABuildable* Buildable) { return Store->BuildDecoration(Buildable); };

  HUD->SetAndOpenBuildableDisplay(Buildable, BuildStockDisplayFunc, BuildDecorationFunc);
}

void APlayerZDCharacter::EnterStockDisplay(UStockDisplayComponent* StockDisplayC,
                                           UInventoryComponent* DisplayInventoryC) {
  auto PlayerToDisplayFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    // ? Cache the stock display inventories?
    bool bSuccess = TransferItem(SourceInventory, DisplayInventoryC, DroppedItem).bSuccess;
    if (bSuccess) {
      StockDisplayC->SetDisplaySprite(DroppedItem->AssetData.Sprite);
      Store->InitStockDisplays();
    }

    return bSuccess;
  };
  auto DisplayToPlayerFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == DisplayInventoryC);

    bool bSuccess = TransferItem(SourceInventory, PlayerInventoryComponent, DroppedItem).bSuccess;
    if (bSuccess) {
      StockDisplayC->ClearDisplaySprite();
      Store->InitStockDisplays();
    }

    return bSuccess;
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
  auto StoreToPlayerFunc = [this, NpcStoreC, StoreInventoryC](UItemBase* DroppedItem,
                                                              UInventoryComponent* SourceInventory) {
    check(SourceInventory == StoreInventoryC);
    return Market->BuyItem(NpcStoreC, SourceInventory, PlayerInventoryComponent, Store, DroppedItem, 1);
  };
  auto PlayerToStoreFunc = [this, NpcStoreC, StoreInventoryC](UItemBase* DroppedItem,
                                                              UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    return Market->SellItem(NpcStoreC, StoreInventoryC, SourceInventory, Store, DroppedItem, 1);
  };

  HUD->SetAndOpenNPCStore(StoreInventoryC, PlayerInventoryComponent, PlayerToStoreFunc, StoreToPlayerFunc);
}

void APlayerZDCharacter::EnterDialogue(UDialogueComponent* DialogueC, std::function<void()> OnDialogueEndFunc) {
  if (DialogueC->DialogueArray.Num() == 0) {
    if (OnDialogueEndFunc) return OnDialogueEndFunc();
    return;
  }

  auto CurrentDialogueArr = DialogueC->GetNextDialogueChain();
  DialogueSystem->StartDialogue(CurrentDialogueArr);
  HUD->SetAndOpenDialogue(DialogueSystem, [this, DialogueC, OnDialogueEndFunc]() {
    // ? Call in dialogue system?
    DialogueC->FinishReadingDialogueChain();

    if (OnDialogueEndFunc) OnDialogueEndFunc();
  });
}
// For dialogue outside of the dialogue component (cutscenes, etc.).
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
                                    USimpleSpriteAnimComponent* SpriteAnimC,
                                    UCustomerAIComponent* CustomerAI,
                                    UItemBase* Item) {
  // * Differentiate between when quest is from a customer (store open), and from a npc (e.g., in market).
  if (!CustomerAI)
    return EnterDialogue(DialogueC, [this, QuestC, SpriteAnimC, CustomerAI]() {
      QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
      SpriteAnimC->ReturnToOgRotation();
    });

  switch (QuestC->CustomerAction) {
    case ECustomerAction::PickItem:
    case ECustomerAction::StockCheck:
    case ECustomerAction::SellItem:
      check(CustomerAI->NegotiationAI->RelevantItem);
      if (QuestC->QuestOutcomeType == EQuestOutcomeType::Negotiation)
        EnterDialogue(DialogueC,
                      [this, Item, CustomerAI, QuestC]() { EnterNegotiation(CustomerAI, Item, true, QuestC); });
      else
        EnterDialogue(DialogueC, [this, QuestC, CustomerAI, Item]() {
          QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
          EnterNegotiation(CustomerAI, Item);
        });
      break;
    case ECustomerAction::Leave:
    case ECustomerAction::None:
      if (CustomerAI->CustomerState == ECustomerState::Leaving) return;
      EnterDialogue(DialogueC, [this, QuestC, CustomerAI]() {
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

// ? All player states handled?
void APlayerZDCharacter::ExitCurrentAction() {
  if (PlayerBehaviourState == EPlayerState::Normal) return;

  // * Should never be called in cutscene state.
  // ? Implement a queue to exit after cutscene?
  check(PlayerBehaviourState != EPlayerState::Cutscene);

  HUD->PlayerCloseAllMenus();
  ChangePlayerState(EPlayerState::Normal);
}

// ? Change parameter to ELevel?
void APlayerZDCharacter::EnterNewLevel(ULevelChangeComponent* LevelChangeC) {
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
void APlayerZDCharacter::LeaveStore() {
  ELevel LevelToLoad = ELevel::Market;
  switch (StorePhaseManager->StorePhaseState) {
    case EStorePhaseState::Morning:
    case EStorePhaseState::MorningBuildMode: {
      LevelToLoad = ELevel::Market;
      break;
    }
    case EStorePhaseState::ShopOpen: return;
    case EStorePhaseState::Night: LevelToLoad = ELevel::Church; break;
  }

  // ? Put in level manager?
  auto LevelReadyFunc = [this, LevelToLoad]() {
    ASpawnPoint* SpawnPoint = *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)
                                   .FindByPredicate([LevelToLoad](const ASpawnPoint* SpawnPoint) {
                                     return SpawnPoint->Level == LevelToLoad;
                                   });
    check(SpawnPoint);

    this->SetActorLocation(SpawnPoint->GetActorLocation());
  };
  LevelManager->BeginLoadLevel(LevelToLoad, LevelReadyFunc);
}