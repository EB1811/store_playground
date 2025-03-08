#include "GlobalDataManager.h"
#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"

AGlobalDataManager::AGlobalDataManager() { PrimaryActorTick.bCanEverTick = false; }

void AGlobalDataManager::BeginPlay() {
  Super::BeginPlay();

  check(UniqueNpcDataTable && UniqueNpcDialoguesTable && CustomerDialoguesTable &&
        FriendlyNegDialoguesTable.DataTable && NeutralNegDialoguesTable.DataTable &&
        HostileNegDialoguesTable.DataTable);

  InitializeNPCData();
  InitializeDialogueData();
}

void AGlobalDataManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

FUniqueNpcData AGlobalDataManager::GetRandomUniqueNpcData() const {
  int32 MapSize = UniqueNpcArray.Num();
  int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
  return UniqueNpcArray[RandomIndex];
}

TArray<struct FDialogueData> AGlobalDataManager::GetRandomCustomerDialogue() const {
  TArray<struct FDialogueData> RandomDialogue;

  int32 MapSize = CustomerDialogues.Num();
  int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
  for (int32 i = RandomIndex - 1; i >= 0; i--) {
    if (CustomerDialogues[i].DialogueChainID != CustomerDialogues[RandomIndex].DialogueChainID) break;
    RandomIndex = i;
  }

  for (int32 i = RandomIndex; i < MapSize; i++) {
    if (CustomerDialogues[i].DialogueChainID != CustomerDialogues[RandomIndex].DialogueChainID) break;
    RandomDialogue.Add(CustomerDialogues[i]);
  }

  return RandomDialogue;
}

TMap<ENegotiationDialogueType, FDialoguesArray> AGlobalDataManager::GetRandomNegDialogueMap(
    ECustomerAttitude Attitude) const {
  auto& DialoguesMap = Attitude == ECustomerAttitude::Friendly  ? FriendlyDialoguesMap
                       : Attitude == ECustomerAttitude::Neutral ? NeutralDialoguesMap
                                                                : HostileDialoguesMap;

  TMap<ENegotiationDialogueType, FDialoguesArray> RandomDialogueIndexMap;
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) {
    RandomDialogueIndexMap.Add(Type, {});

    auto& Dialogues = DialoguesMap[Type].Dialogues;
    int32 MapSize = Dialogues.Num();
    int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
    for (int32 i = RandomIndex - 1; i >= 0; i--) {
      if (Dialogues[i].DialogueChainID != Dialogues[RandomIndex].DialogueChainID) break;
      RandomIndex = i;
    }

    for (int32 i = RandomIndex; i < MapSize; i++) {
      if (Dialogues[i].DialogueChainID != Dialogues[RandomIndex].DialogueChainID) break;
      RandomDialogueIndexMap[Type].Dialogues.Add(Dialogues[i]);
    }
  }
  return RandomDialogueIndexMap;
}

void AGlobalDataManager::InitializeNPCData() {
  UniqueNpcArray.Empty();
  TArray<FUniqueNpcDataTable*> UniqueNpcRows;
  UniqueNpcDataTable->GetAllRows<FUniqueNpcDataTable>("", UniqueNpcRows);
  for (auto* Row : UniqueNpcRows)
    UniqueNpcArray.Add({
        Row->NpcID,
        Row->SpawnChanceMultiplier,
        Row->DialogueChainIDs,
        Row->WantedBaseItemIDs,
        Row->AcceptancePercentageRange,
        Row->AssetData,
    });

  UE_LOG(LogTemp, Warning, TEXT("UniqueNpc Data: %d"), UniqueNpcArray.Num());

  UniqueNpcDataTable = nullptr;
}

void AGlobalDataManager::InitializeDialogueData() {
  UniqueNpcDialoguesMap.Empty();
  TArray<FDialogueDataTable*> UniqueNpcDialoguesRows;
  UniqueNpcDialoguesTable->GetAllRows<FDialogueDataTable>("", UniqueNpcDialoguesRows);
  for (auto* Row : UniqueNpcDialoguesRows) {
    UniqueNpcDialoguesMap.FindOrAdd(Row->DialogueChainID, {});
    UniqueNpcDialoguesMap[Row->DialogueChainID].Dialogues.Add({
        Row->DialogueChainID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });
  }

  CustomerDialogues.Empty();
  TArray<FDialogueDataTable*> CustomerDialoguesRows;
  CustomerDialoguesTable->GetAllRows<FDialogueDataTable>("", CustomerDialoguesRows);
  for (auto* Row : CustomerDialoguesRows)
    CustomerDialogues.Add({
        Row->DialogueChainID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });

  FriendlyDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) FriendlyDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> FriendlyRows;
  FriendlyNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(FriendlyRows, TEXT("Friendly Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : FriendlyRows)
    FriendlyDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueType,
                                                              Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                              Row->ChoicesAmount});

  NeutralDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) NeutralDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> NeutralRows;
  NeutralNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(NeutralRows, TEXT("Neutral Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : NeutralRows)
    NeutralDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueType, Row->DialogueText,
                                                             Row->Action, Row->DialogueSpeaker, Row->ChoicesAmount});

  HostileDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) HostileDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> HostileRows;
  HostileNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(HostileRows, TEXT("Hostile Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : HostileRows)
    HostileDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueType, Row->DialogueText,
                                                             Row->Action, Row->DialogueSpeaker, Row->ChoicesAmount});

  UE_LOG(LogTemp, Warning, TEXT("UniqueNpc Dialogues: %d"), UniqueNpcDialoguesMap.Num());
  UE_LOG(LogTemp, Warning, TEXT("Customer Dialogues: %d"), CustomerDialogues.Num());
  UE_LOG(LogTemp, Warning, TEXT("Neutral Dialogues: %d"),
         NeutralDialoguesMap[ENegotiationDialogueType::Request].Dialogues.Num());
  UE_LOG(LogTemp, Warning, TEXT("Friendly Dialogues: %d"),
         FriendlyDialoguesMap[ENegotiationDialogueType::Request].Dialogues.Num());
  UE_LOG(LogTemp, Warning, TEXT("Hostile Dialogues: %d"),
         HostileDialoguesMap[ENegotiationDialogueType::Request].Dialogues.Num());

  UniqueNpcDialoguesTable = nullptr;
  CustomerDialoguesTable = nullptr;
  FriendlyNegDialoguesTable.DataTable = nullptr;
  NeutralNegDialoguesTable.DataTable = nullptr;
  HostileNegDialoguesTable.DataTable = nullptr;
}