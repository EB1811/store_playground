// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
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
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Level/LevelChangeComponent.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/UI/SpgHUD.h"
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

  InteractionCheckDistance = 200.0f;

  PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void APlayerZDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerZDCharacter::Move);

    EnhancedInputComponent->BindAction(CloseTopOpenMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::CloseTopOpenMenu);
    EnhancedInputComponent->BindAction(CloseAllMenusAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::CloseAllMenus);
    EnhancedInputComponent->BindAction(OpenInventoryViewAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenInventoryView);
    EnhancedInputComponent->BindAction(BuildModeAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::EnterBuildMode);
    EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerZDCharacter::Interact);
  }
}

void APlayerZDCharacter::BeginPlay() {
  Super::BeginPlay();

  check(SpawnPointClass);

  if (GetWorld()->GetFirstPlayerController()) {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
            GetWorld()->GetFirstPlayerController()->GetLocalPlayer())) {
      Subsystem->AddMappingContext(InputMappingContext, 0);
    }
  }

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void APlayerZDCharacter::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

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

void APlayerZDCharacter::Interact(const FInputActionValue& Value) {
  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart + GetActorRotation().Vector() * InteractionCheckDistance};

  DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  FHitResult TraceHit;

  if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
    if ((TraceStart - TraceHit.ImpactPoint).Size() <= InteractionCheckDistance) {
      if (UInteractionComponent* Interactable = TraceHit.GetActor()->FindComponentByClass<UInteractionComponent>()) {
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
            if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning) break;

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
            auto [CustomerAI, Dialogue, Item] = Interactable->InteractUniqueNPCQuest();
            check(CustomerAI->QuestChainData.QuestID != NAME_None);

            // TODO: Put CustomerAction switch in function.
            switch (CustomerAI->CustomerAction) {
              case ECustomerAction::PickItem:
              case ECustomerAction::StockCheck:
              case ECustomerAction::SellItem:
                check(CustomerAI->NegotiationAI->RelevantItem);
                if (CustomerAI->QuestChainData.QuestChainType == EQuestChainType::Negotiation)
                  EnterDialogue(Dialogue->DialogueArray,
                                [this, Item, CustomerAI]() { EnterNegotiation(CustomerAI, Item, true); });
                else
                  EnterDialogue(Dialogue->DialogueArray, [this, Item, CustomerAI]() {
                    CustomerAIManager->CompleteQuestChain(CustomerAI->QuestChainData,
                                                          DialogueSystem->ChoiceDialoguesSelectedIDs);
                    EnterNegotiation(CustomerAI, Item);
                  });
                break;
              case ECustomerAction::None:
              case ECustomerAction::Leave:
                EnterDialogue(Dialogue->DialogueArray, [this, CustomerAI]() {
                  CustomerAIManager->CompleteQuestChain(CustomerAI->QuestChainData,
                                                        DialogueSystem->ChoiceDialoguesSelectedIDs);
                  CustomerAI->CustomerState = ECustomerState::Leaving;
                });
                break;
            }
            break;
          }
          case EInteractionType::NpcStore: {
            if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning) break;

            auto [NpcStoreC, StoreInventory, Dialogue] = Interactable->InteractNpcStore();
            EnterDialogue(Dialogue->DialogueArray,
                          [this, NpcStoreC, StoreInventory]() { EnterNpcStore(NpcStoreC, StoreInventory); });
            break;
          }
          case EInteractionType::Container: {
            UInventoryComponent* ContainerInventory = Interactable->InteractContainer();
            HUD->SetAndOpenContainer(PlayerInventoryComponent, ContainerInventory);
            break;
          }
        }
      }
    }
  }
}

void APlayerZDCharacter::EnterBuildableDisplay(ABuildable* Buildable) {
  auto BuildStockDisplayFunc = [this](ABuildable* Buildable) {
    if (Buildable->BuildingPricesMap[EBuildableType::StockDisplay] > Store->Money) return false;

    Buildable->SetToStockDisplay();
    if (Buildable->BuildableType != EBuildableType::StockDisplay) return false;

    Store->Money -= Buildable->BuildingPricesMap[EBuildableType::StockDisplay];
    return true;
  };
  auto BuildDecorationFunc = [this](ABuildable* Buildable) {
    if (Buildable->BuildingPricesMap[EBuildableType::Decoration] > Store->Money) return false;

    Buildable->SetToDecoration();
    if (Buildable->BuildableType != EBuildableType::Decoration) return false;

    Store->Money -= Buildable->BuildingPricesMap[EBuildableType::Decoration];
    return true;
  };

  HUD->SetAndOpenBuildableDisplay(Buildable, BuildStockDisplayFunc, BuildDecorationFunc);
}

void APlayerZDCharacter::EnterStockDisplay(UStockDisplayComponent* StockDisplayC,
                                           UInventoryComponent* DisplayInventoryC) {
  auto PlayerToDisplayFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    // ? Have a function in the store to refetch all stock?
    if (TransferItem(SourceInventory, DisplayInventoryC, DroppedItem).bSuccess)
      Store->StoreStockItems.Add(
          {DisplayInventoryC, DroppedItem});  // todo-low: Check if the two item instances cause problems.
  };
  auto DisplayToPlayerFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == DisplayInventoryC);
    if (TransferItem(SourceInventory, PlayerInventoryComponent, DroppedItem).bSuccess)
      Store->StoreStockItems.RemoveAll(
          [DroppedItem](const FStockItem& StockItem) { return StockItem.Item == DroppedItem; });
  };

  HUD->SetAndOpenStockDisplay(StockDisplayC, DisplayInventoryC, PlayerInventoryComponent, PlayerToDisplayFunc,
                              DisplayToPlayerFunc);
}

void APlayerZDCharacter::EnterDialogue(const TArray<FDialogueData> DialogueDataArr,
                                       std::function<void()> OnDialogueEndFunc) {
  DialogueSystem->StartDialogue(DialogueDataArr);
  HUD->SetAndOpenDialogue(DialogueSystem, OnDialogueEndFunc);
}

void APlayerZDCharacter::EnterNegotiation(UCustomerAIComponent* CustomerAI,
                                          const UItemBase* Item,
                                          bool bIsQuestAssociated) {
  NegotiationSystem->StartNegotiation(CustomerAI, Item, CustomerAI->NegotiationAI->StockDisplayInventory,
                                      bIsQuestAssociated);
  HUD->SetAndOpenNegotiation(NegotiationSystem, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterNpcStore(UNpcStoreComponent* NpcStoreC, UInventoryComponent* StoreInventoryC) {
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
    case EStorePhaseState::Night: return;
  }

  auto LevelReadyFunc = [this, LevelChangeC]() {
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPointClass, FoundActors);
    ASpawnPoint* SpawnPoint = Cast<ASpawnPoint>(*(FoundActors.FindByPredicate(
        [LevelChangeC](const AActor* Actor) { return Cast<ASpawnPoint>(Actor)->Level == LevelChangeC->LevelToLoad; })));
    check(SpawnPoint);

    this->SetActorLocation(SpawnPoint->GetActorLocation());
  };

  LevelManager->BeginLoadLevel(LevelChangeC->LevelToLoad, LevelReadyFunc);
}