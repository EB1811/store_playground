#include "CustomerAIComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "store_playground/WorldObject/Customer.h"
#include "Components/WidgetComponent.h"
#include "PaperZDCharacter.h"
#include "AIController.h"

UCustomerAIComponent::UCustomerAIComponent() {
  PrimaryComponentTick.bCanEverTick = false;
  PrimaryComponentTick.TickInterval = 5.0f;
}
void UCustomerAIComponent::BeginPlay() {
  Super::BeginPlay();

  CustomerAIID = FGuid::NewGuid();
}
void UCustomerAIComponent::TickComponent(float DeltaTime,
                                         ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCustomerAIComponent::StartDialogue() { CustomerState = ECustomerState::BrowsingTalking; }
void UCustomerAIComponent::LeaveDialogue() { CustomerState = ECustomerState::Browsing; }

void UCustomerAIComponent::StartQuest() {
  CustomerState = ECustomerState::PerformingQuest;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(false, true);
}
void UCustomerAIComponent::LeaveQuest() {
  CustomerState = ECustomerState::RequestingQuest;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(true, true);
}
void UCustomerAIComponent::FinishQuest() {
  CustomerState = ECustomerState::Leaving;

  UInteractionComponent* OwnerInteraction = GetOwner()->FindComponentByClass<UInteractionComponent>();
  check(OwnerInteraction);
  OwnerInteraction->InteractionType = EInteractionType::None;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(false, true);
}

void UCustomerAIComponent::StartRequestDialogue() {
  CustomerState = ECustomerState::RequestingTalking;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(false, true);
}
void UCustomerAIComponent::LeaveRequestDialogue() {
  CustomerState = ECustomerState::Requesting;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(true, true);
}
void UCustomerAIComponent::StartNegotiation() { CustomerState = ECustomerState::Negotiating; }
void UCustomerAIComponent::PostNegotiation() {
  CustomerState = ECustomerState::Leaving;

  UInteractionComponent* OwnerInteraction = GetOwner()->FindComponentByClass<UInteractionComponent>();
  check(OwnerInteraction);
  OwnerInteraction->InteractionType = EInteractionType::None;
}