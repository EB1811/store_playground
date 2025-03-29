#include "QuestManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/UtilFuncs.h"

AQuestManager::AQuestManager() { PrimaryActorTick.bCanEverTick = false; }

void AQuestManager::BeginPlay() { Super::BeginPlay(); }

void AQuestManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

TArray<struct FQuestChainData> AQuestManager::GetEligibleQuestChains(const TArray<FName>& QuestIDs) const {
  const TMap<EReqFilterOperand, std::any> GameDataMap = {{}};  // Temp.

  TMap<FName, FName> PrevChainCompletedMap = {};
  for (const auto& QuestInProgress : QuestInProgressMap)
    PrevChainCompletedMap.Add(QuestInProgress.Key, QuestInProgress.Value.ChainCompletedIDs.Last());

  return GlobalDataManager->GetEligibleQuestChains(QuestIDs, GameDataMap, QuestsCompleted, PrevChainCompletedMap);
}

void AQuestManager::CompleteQuestChain(const FQuestChainData& QuestChainData,
                                       TArray<FName> MadeChoiceIds,
                                       bool bNegotiationSuccess) {
  if (QuestsCompleted.Contains(QuestChainData.QuestID)) return;

  switch (QuestChainData.QuestAction) {
    case EQuestAction::Continue: {
      QuestInProgressMap.FindOrAdd(QuestChainData.QuestID, {});
      if (QuestInProgressMap[QuestChainData.QuestID].ChainCompletedIDs.Contains(QuestChainData.QuestChainID)) return;

      QuestInProgressMap[QuestChainData.QuestID].ChainCompletedIDs.Add(QuestChainData.QuestChainID);

      if (QuestChainData.QuestChainType == EQuestChainType::DialogueChoice)
        QuestInProgressMap[QuestChainData.QuestID].ChoicesMade.Append(MadeChoiceIds);

      if (QuestChainData.QuestChainType == EQuestChainType::Negotiation)
        QuestInProgressMap[QuestChainData.QuestID].NegotiationOutcomesMap.Add(QuestChainData.QuestChainID,
                                                                              bNegotiationSuccess);
      return;
    }
    case EQuestAction::End: {
      QuestsCompleted.Add(QuestChainData.QuestChainID);
      if (QuestInProgressMap.Contains(QuestChainData.QuestID)) QuestInProgressMap.Remove(QuestChainData.QuestID);
      return;
    }
    default: return;
  }
}
