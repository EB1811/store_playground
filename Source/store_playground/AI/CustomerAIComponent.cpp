#include "CustomerAIComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NegotiationAI.h"
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
  NegotiationAI = NewObject<UNegotiationAI>(this);
  Attitude = ECustomerAttitude::Neutral;
  CustomerState = ECustomerState::Browsing;
  CustomerAction = ECustomerAction::None;
}
void UCustomerAIComponent::TickComponent(float DeltaTime,
                                         ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCustomerAIComponent::LeaveRequestDialogue() {
  CustomerState = ECustomerState::Requesting;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(true, true);

  USimpleSpriteAnimComponent* SpriteAnimC = GetOwner()->FindComponentByClass<USimpleSpriteAnimComponent>();
  check(SpriteAnimC);
  SpriteAnimC->ReturnToOgRotation();
}

void UCustomerAIComponent::StartNegotiation() {
  CustomerState = ECustomerState::Negotiating;

  UWidgetComponent* WidgetC = GetOwner()->FindComponentByClass<UWidgetComponent>();
  check(WidgetC);
  WidgetC->SetVisibility(false, true);
}

void UCustomerAIComponent::PostNegotiation() {
  CustomerState = ECustomerState::Leaving;

  UInteractionComponent* OwnerInteraction = GetOwner()->FindComponentByClass<UInteractionComponent>();
  check(OwnerInteraction);
  OwnerInteraction->InteractionType = EInteractionType::None;
}