// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
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
    EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenInventory);
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
  DialogueSystem = NewObject<UDialogueSystem>(this);
  NegotiationSystem = NewObject<UNegotiationSystem>(this);
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

void APlayerZDCharacter::OpenInventory(const FInputActionValue& Value) {
  HUD->SetAndOpenInventory(PlayerInventoryComponent);
}

void APlayerZDCharacter::Interact(const FInputActionValue& Value) {
  FVector TraceStart{GetPawnViewLocation()};
  FVector TraceEnd{TraceStart + GetViewRotation().Vector() * InteractionCheckDistance};

  DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  FHitResult TraceHit;

  if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
    if ((TraceStart - TraceHit.ImpactPoint).Size() <= InteractionCheckDistance) {
      if (UInteractionComponent* Interactable = TraceHit.GetActor()->FindComponentByClass<UInteractionComponent>()) {
        // TODO: Move to separate, free functions.
        // ? Separate out player logic from UI logic, with a UI system class?
        switch (Interactable->InteractionType) {
          case EInteractionType::Use: {
            Interactable->InteractUse();
            break;
          }
          case EInteractionType::NPCDialogue: {
            if (auto DialogueData = Interactable->InteractNPCDialogue()) EnterDialogue(DialogueData.value());
            break;
          }
          case EInteractionType::WaitingCustomer: {
            auto [Item, CustomerAI] = Interactable->InteractWaitingCustomer();
            if (!Item || !CustomerAI) return;

            EnterNegotiation(Item, CustomerAI);
            break;
          }
          case EInteractionType::Store: {
            auto [StoreInventory, StoreMoney] = Interactable->InteractStore();
            if (!StoreInventory) return;

            HUD->SetAndOpenContainer(PlayerInventoryComponent, StoreInventory);
            break;
          }
          case EInteractionType::Container: {
            UInventoryComponent* ContainerInventory = Interactable->InteractContainer();
            if (!ContainerInventory) return;

            HUD->SetAndOpenContainer(PlayerInventoryComponent, ContainerInventory);
            break;
          }
        }
      }
    }
  }
}

void APlayerZDCharacter::EnterDialogue(const TArray<FDialogueData> DialogueDataArr) {
  DialogueSystem->StartDialogue(DialogueDataArr);
  HUD->SetAndOpenDialogue(DialogueSystem);
}

// ? Move to HUD?
void APlayerZDCharacter::EnterNegotiation(const UItemBase* Item, const UCustomerAIComponent* CustomerAI) {
  NegotiationSystem->StartNegotiation(Item, CustomerAI->NegotiationAI, PlayerInventoryComponent,
                                      Item->MarketData.BasePrice);
  HUD->SetAndOpenNegotiation(NegotiationSystem);
}
