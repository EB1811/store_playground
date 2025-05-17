// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "store_playground/AI/NegotiationAI.h"
#include "NegotiationSystem.generated.h"

UENUM()
enum class Negotiator : uint8 {
  Player,
  NPC,
};
UENUM()
enum class NegotiationType : uint8 {
  PlayerBuy UMETA(DisplayName = "Player Buy"),
  PlayerSell UMETA(DisplayName = "Player Sell"),
};
// ? Create a stock check system?
UENUM()
enum class ENegotiationState : uint8 {
  None UMETA(DisplayName = "NONE"),
  // * Npc requests item.
  NpcRequest UMETA(DisplayName = "NPC Request"),
  // * Alternatively, stock check.
  NpcStockCheckRequest UMETA(DisplayName = "NPC Request Stock Check"),
  PlayerStockCheck UMETA(DisplayName = "Player Stock Check"),
  NpcStockCheckConsider UMETA(DisplayName = "NPC Stock Check Consider"),

  // * Negotiation back and forth.
  PlayerConsider UMETA(DisplayName = "Player Consider"),
  NpcConsider UMETA(DisplayName = "NPC Consider"),
  Accepted UMETA(DisplayName = "Accepted"),
  Rejected UMETA(DisplayName = "Rejected"),
};
UENUM()
enum class ENegotiationAction : uint8 {
  NpcRequest UMETA(DisplayName = "NPC Request"),
  PlayerReadRequest UMETA(DisplayName = "Player Read Request"),
  NpcStockCheckRequest UMETA(DisplayName = "NPC Stock Check Request"),
  PlayerShowItem UMETA(DisplayName = "Player Stock Check Show Item"),
  OfferPrice UMETA(DisplayName = "Offer Price"),
  Accept UMETA(DisplayName = "Accept"),
  Reject UMETA(DisplayName = "Reject")
};
USTRUCT()
struct FNStateAction {
  GENERATED_BODY()

  ENegotiationState initial;
  ENegotiationAction action;
  ENegotiationState next;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API UNegotiationSystem : public UObject {
  GENERATED_BODY()

public:
  UNegotiationSystem();

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  TArray<FNStateAction> NStateTransitions;
  ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action);

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UDialogueSystem* DialogueSystem;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UInventoryComponent* PlayerInventory;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class AQuestManager* QuestManager;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float BoughtAtPrice;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float MarketPrice;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  ENegotiationState NegotiationState;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float OfferedPrice;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  TArray<class UItemBase*> NegotiatedItems;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UInventoryComponent* FromInventory;  // ? Remove from here?
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  NegotiationType Type;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  FWantedItemType WantedItemType;  // ? Move stock check to another system?

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UCustomerAIComponent* CustomerAI;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  bool bIsQuestAssociated;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UQuestComponent* QuestComponent;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  FOfferResponse CustomerOfferResponse;

  void StartNegotiation(class UCustomerAIComponent* _CustomerAI,
                        class UItemBase* NegotiatedItem = nullptr,
                        class UInventoryComponent* _FromInventory = nullptr,
                        bool _bIsQuestAssociated = false,
                        UQuestComponent* _QuestComponent = nullptr,
                        ENegotiationState InitState = ENegotiationState::None);

  struct FNextDialogueRes NPCRequestNegotiation();
  void PlayerReadRequest();
  void PlayerShowItem(class UItemBase* Item, class UInventoryComponent* _FromInventory);
  struct FNextDialogueRes NPCNegotiationTurn();
  void OfferPrice(float Price);
  void AcceptOffer();
  void RejectOffer();

  void NegotiationSuccess();
  void NegotiationFailure();
};
