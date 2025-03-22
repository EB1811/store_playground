#include "CustomerAIComponent.h"
#include "NegotiationAI.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

UCustomerAIComponent::UCustomerAIComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  CustomerID = FGuid::NewGuid();
  CustomerType = ECustomerType::Generic;
}

void UCustomerAIComponent::BeginPlay() {
  Super::BeginPlay();

  NegotiationAI = NewObject<UNegotiationAI>(this);
  Attitude = ECustomerAttitude::Neutral;
  CustomerState = ECustomerState::Browsing;
  CustomerAction = ECustomerAction::None;
}

void UCustomerAIComponent::StartNegotiation() { CustomerState = ECustomerState::Negotiating; }

void UCustomerAIComponent::PostNegotiation() {
  CustomerState = ECustomerState::Leaving;

  UInteractionComponent* OwnerInteraction = GetOwner()->FindComponentByClass<UInteractionComponent>();
  check(OwnerInteraction);
  OwnerInteraction->InteractionType = EInteractionType::None;
}