// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"

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
    EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerZDCharacter::Interact);
  }
}

void APlayerZDCharacter::BeginPlay() {
  Super::BeginPlay();

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
}

void APlayerZDCharacter::CloseTopOpenMenu(const FInputActionValue& Value) { HUD->CloseTopOpenMenu(); }

void APlayerZDCharacter::CloseAllMenus(const FInputActionValue& Value) { HUD->CloseAllMenus(); }

void APlayerZDCharacter::OpenInventoryView(const FInputActionValue& Value) {
  HUD->SetAndOpenInventoryView(PlayerInventoryComponent, Store);
}

void APlayerZDCharacter::Interact(const FInputActionValue& Value) {
  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart + GetViewRotation().Vector() * InteractionCheckDistance};

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
          case EInteractionType::StoreNextPhase: {
            StorePhaseManager->NextPhase();
            break;
          }
          case EInteractionType::Use: {
            Interactable->InteractUse();
            break;
          }
          case EInteractionType::StockDisplay: {
            if (StorePhaseManager->ShopPhaseState != EShopPhaseState::Morning) break;

            auto [DisplayC, DisplayInventoryC] = Interactable->InteractStockDisplay();
            EnterStockDisplay(DisplayC, DisplayInventoryC);
            break;
          }
          case EInteractionType::NPCDialogue: {
            auto DialogueData = Interactable->InteractNPCDialogue();
            EnterDialogue(DialogueData.value());
            break;
          }
          case EInteractionType::WaitingCustomer: {
            auto [Item, CustomerAI] = Interactable->InteractWaitingCustomer();
            EnterNegotiation(Item, CustomerAI);
            break;
          }
          case EInteractionType::WaitingUniqueCustomer: {
            auto [Item, CustomerAI, Dialogue] = Interactable->InteractWaitingUniqueCustomer();
            EnterDialogue(Dialogue->DialogueArray, [this, Item, CustomerAI]() { EnterNegotiation(Item, CustomerAI); });
            break;
          }
          case EInteractionType::NpcStore: {
            if (StorePhaseManager->ShopPhaseState != EShopPhaseState::Morning) break;

            auto [StoreInventory, Dialogue] = Interactable->InteractNpcStore();
            EnterDialogue(Dialogue->DialogueArray, [this, StoreInventory]() { EnterNpcStore(StoreInventory); });
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

void APlayerZDCharacter::EnterStockDisplay(UStockDisplayComponent* StockDisplayC,
                                           UInventoryComponent* DisplayInventoryC) {
  auto PlayerToDisplayFunc = [this, StockDisplayC, DisplayInventoryC](UItemBase* DroppedItem,
                                                                      UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    // ? Have a function in the store to refetch all stock?
    if (TransferItem(SourceInventory, DisplayInventoryC, DroppedItem).bSuccess)
      Store->StoreStockItems.Add({DisplayInventoryC, DroppedItem});
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

void APlayerZDCharacter::EnterNegotiation(const UItemBase* Item, UCustomerAIComponent* CustomerAI) {
  NegotiationSystem->StartNegotiation(Item, CustomerAI, CustomerAI->NegotiationAI->StockDisplayInventory,
                                      Item->MarketData.BasePrice);
  HUD->SetAndOpenNegotiation(NegotiationSystem, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterNpcStore(UInventoryComponent* StoreInventoryC) {
  auto StoreToPlayerFunc = [this, StoreInventoryC](UItemBase* DroppedItem, UInventoryComponent* SourceInventory) {
    check(SourceInventory == StoreInventoryC);
    BuyItem(SourceInventory, PlayerInventoryComponent, Store, DroppedItem, 1);
  };
  auto PlayerToStoreFunc = [this, StoreInventoryC](UItemBase* DroppedItem, UInventoryComponent* SourceInventory) {
    check(SourceInventory == PlayerInventoryComponent);
    SellItem(StoreInventoryC, SourceInventory, Store, DroppedItem, 1);
  };

  HUD->SetAndOpenNPCStore(StoreInventoryC, PlayerInventoryComponent, PlayerToStoreFunc, StoreToPlayerFunc);
}