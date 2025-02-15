#include "CustomerAIComponent.h"
#include "NegotiationAI.h"

UCustomerAIComponent::UCustomerAIComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  CustomerType = ECustomerType::Peasant;
}

void UCustomerAIComponent::BeginPlay() {
  Super::BeginPlay();

  NegotiationAI = NewObject<UNegotiationAI>(this);
  NegotiationAI->Attitude = ENegotiatorAttitude::NEUTRAL;
}