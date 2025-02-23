#include "CustomerAIComponent.h"
#include "NegotiationAI.h"
#include "Engine/DataTable.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> FriendlyDialoguesMap = {};
std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> NeutralDialoguesMap = {};
std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> HostileDialoguesMap = {};

UCustomerAIComponent::UCustomerAIComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  CustomerType = ECustomerType::Peasant;
}

void UCustomerAIComponent::BeginPlay() {
  Super::BeginPlay();

  InitializeDialogueData();

  NegotiationAI = NewObject<UNegotiationAI>(this);
  NegotiationAI->Attitude = ENegotiatorAttitude::FRIENDLY;

  {
    auto& RequestDialogues = FriendlyDialoguesMap[ENegotiationDialogueType::Request];
    int32 MapSize = RequestDialogues.size();
    int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
    for (int32 i = RandomIndex - 1; i >= 0; i--) {
      if (RequestDialogues[i].DialogueChainID != RequestDialogues[RandomIndex].DialogueChainID) break;
      RandomIndex = i;
    }

    for (int32 i = RandomIndex; i < MapSize; i++) {
      if (RequestDialogues[i].DialogueChainID != RequestDialogues[RandomIndex].DialogueChainID) break;

      auto RandomDialogue = RequestDialogues[i];
      NegotiationAI->RequestDialogueArray.Add(RandomDialogue);
    }
  }

  // NegotiationAI->DialogueMap.Add(ENegotiationDialogueType::ConsiderTooHigh,
  //                                FriendlyDialoguesMap[ENegotiationDialogueType::ConsiderTooHigh][0]);
  // NegotiationAI->DialogueMap.Add(ENegotiationDialogueType::ConsiderClose,
  //                                FriendlyDialoguesMap[ENegotiationDialogueType::ConsiderClose][0]);
  // NegotiationAI->DialogueMap.Add(ENegotiationDialogueType::Accept,
  //                                FriendlyDialoguesMap[ENegotiationDialogueType::Accept][0]);
  // NegotiationAI->DialogueMap.Add(ENegotiationDialogueType::Reject,
  //                                FriendlyDialoguesMap[ENegotiationDialogueType::Reject][0]);
}

void UCustomerAIComponent::InitializeDialogueData() {
  FriendlyDialoguesMap.clear();
  FriendlyDialoguesMap[ENegotiationDialogueType::Request] = {};
  FriendlyDialoguesMap[ENegotiationDialogueType::ConsiderTooHigh] = {};
  FriendlyDialoguesMap[ENegotiationDialogueType::ConsiderClose] = {};
  FriendlyDialoguesMap[ENegotiationDialogueType::Accept] = {};
  FriendlyDialoguesMap[ENegotiationDialogueType::Reject] = {};
  TArray<FNegotiationDialoguesDataTable*> FriendlyRows;
  FriendlyDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(FriendlyRows, TEXT("Friendly Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : FriendlyRows)
    FriendlyDialoguesMap[Row->NegotiationType].push_back({Row->DialogueChainID, Row->DialogueType, Row->DialogueText,
                                                          Row->Action, Row->DialogueSpeaker, Row->ChoicesAmount});

  NeutralDialoguesMap.clear();
  NeutralDialoguesMap[ENegotiationDialogueType::Request] = {};
  NeutralDialoguesMap[ENegotiationDialogueType::ConsiderTooHigh] = {};
  NeutralDialoguesMap[ENegotiationDialogueType::ConsiderClose] = {};
  NeutralDialoguesMap[ENegotiationDialogueType::Accept] = {};
  NeutralDialoguesMap[ENegotiationDialogueType::Reject] = {};
  TArray<FNegotiationDialoguesDataTable*> NeutralRows;
  NeutralDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(NeutralRows, TEXT("Neutral Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : NeutralRows)
    NeutralDialoguesMap[Row->NegotiationType].push_back({Row->DialogueChainID, Row->DialogueType, Row->DialogueText,
                                                         Row->Action, Row->DialogueSpeaker, Row->ChoicesAmount});

  HostileDialoguesMap.clear();
  HostileDialoguesMap[ENegotiationDialogueType::Request] = {};
  HostileDialoguesMap[ENegotiationDialogueType::ConsiderTooHigh] = {};
  HostileDialoguesMap[ENegotiationDialogueType::ConsiderClose] = {};
  HostileDialoguesMap[ENegotiationDialogueType::Accept] = {};
  HostileDialoguesMap[ENegotiationDialogueType::Reject] = {};
  TArray<FNegotiationDialoguesDataTable*> HostileRows;
  HostileDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(HostileRows, TEXT("Hostile Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : HostileRows)
    HostileDialoguesMap[Row->NegotiationType].push_back({Row->DialogueChainID, Row->DialogueType, Row->DialogueText,
                                                         Row->Action, Row->DialogueSpeaker, Row->ChoicesAmount});

  UE_LOG(LogTemp, Warning, TEXT("Neutral Dialogues: %d"),
         NeutralDialoguesMap[ENegotiationDialogueType::Request].size());
  UE_LOG(LogTemp, Warning, TEXT("Friendly Dialogues: %d"),
         FriendlyDialoguesMap[ENegotiationDialogueType::Request].size());
  UE_LOG(LogTemp, Warning, TEXT("Hostile Dialogues: %d"),
         HostileDialoguesMap[ENegotiationDialogueType::Request].size());
}