
#include "CustomerAIManager.h"
#include "CustomerAIComponent.h"
#include "NegotiationAI.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "store_playground/WorldObject/Customer.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"

std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> FriendlyDialoguesMap = {};
std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> NeutralDialoguesMap = {};
std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> HostileDialoguesMap = {};

std::unordered_map<ENegotiationDialogueType, std::vector<int32>> GetRandomNegDialogues() {
  std::unordered_map<ENegotiationDialogueType, std::vector<int32>> DialogueIndexMap = {};

  DialogueIndexMap[ENegotiationDialogueType::Request] = {};
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
      DialogueIndexMap[ENegotiationDialogueType::Request].push_back(i);
    }
  }

  return DialogueIndexMap;
}

ACustomerAIManager::ACustomerAIManager() {
  PrimaryActorTick.bCanEverTick = true;

  CustomerSpawnInterval = 5.0f;
  MaxCustomers = 1;
  PickItemChance = 50.0f;
  PickItemFrequency = 5.0f;
  LastPickItemCheckTime = 0.0f;
}

void ACustomerAIManager::BeginPlay() {
  Super::BeginPlay();

  InitializeDialogueData();
  SpawnCustomers();
}

void ACustomerAIManager::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (GetWorld()->TimeSince(LastPickItemCheckTime) > PickItemFrequency) PerformCustomerAILoop();
}

// TODO: Monitor performance.
void ACustomerAIManager::PerformCustomerAILoop() {
  check(PlayerStock);
  UE_LOG(LogTemp, Warning, TEXT("PerformCustomerAILoop"));

  LastPickItemCheckTime = GetWorld()->GetTimeSeconds();

  for (ACustomer* Customer : AllCustomers) {
    switch (Customer->CustomerAIComponent->CustomerState) {
      case (ECustomerState::Browsing): {
        if (FMath::FRand() * 100 < PickItemChance) CustomerPickItem(Customer);
        break;
      }
      default: break;
    }
  }
}

// TODO: Add check for already picked items.
void ACustomerAIManager::CustomerPickItem(class ACustomer* Customer) {
  if (PlayerStock->ItemsArray.Num() <= 0) return;

  Customer->CustomerAIComponent->CustomerState = ECustomerState::ItemWanted;

  UItemBase* Item = PlayerStock->ItemsArray[FMath::RandRange(0, PlayerStock->ItemsArray.Num() - 1)];
  Customer->CustomerAIComponent->NegotiationAI->WantedItem = Item;
  Customer->InteractionComponent->InteractionType = EInteractionType::WaitingCustomer;

  auto DialogueIndexMap = GetRandomNegDialogues();
  for (int32 RandomDialogue : DialogueIndexMap[ENegotiationDialogueType::Request])
    Customer->CustomerAIComponent->NegotiationAI->RequestDialogueArray.Add(
        FriendlyDialoguesMap[ENegotiationDialogueType::Request][RandomDialogue]);
}

void ACustomerAIManager::SpawnCustomers() {
  check(CustomerClass);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  for (int32 i = 0; i < MaxCustomers; i++) {
    ACustomer* Customer = GetWorld()->SpawnActor<ACustomer>(
        CustomerClass, GetActorLocation() + (GetActorForwardVector() + 50.0f), GetActorRotation(), SpawnParams);

    Customer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
    Customer->CustomerAIComponent->CustomerType = ECustomerType::Peasant;

    // ? Maybe have two arrays for customers ai and customer interaction components?
    AllCustomers.Add(Customer);
  }
}

void ACustomerAIManager::InitializeDialogueData() {
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