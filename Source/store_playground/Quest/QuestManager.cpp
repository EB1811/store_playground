#include "QuestManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/WorldObject/Npc.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"

AQuestManager::AQuestManager() { PrimaryActorTick.bCanEverTick = false; }

void AQuestManager::BeginPlay() { Super::BeginPlay(); }

void AQuestManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

TArray<struct FQuestChainData> AQuestManager::GetEligibleQuestChains(const TArray<FName>& QuestIDs) const {
  TMap<FName, FName> PrevChainCompletedMap = {};
  for (const auto& QuestInProgress : QuestInProgressMap)
    PrevChainCompletedMap.Add(QuestInProgress.Key, QuestInProgress.Value.ChainCompletedIDs.Last());

  return GlobalStaticDataManager->GetEligibleQuestChains(QuestIDs, QuestsCompleted, PrevChainCompletedMap);
}

void AQuestManager::CompleteQuestChain(UQuestComponent* QuestC, TArray<FName> MadeChoiceIds, bool bNegotiationSuccess) {
  const FQuestChainData& QuestChainData = GlobalStaticDataManager->GetQuestChainById(QuestC->ChainID);
  if (QuestsCompleted.Contains(QuestChainData.QuestID)) return;

  switch (QuestChainData.QuestAction) {
    case EQuestAction::Continue:
    case EQuestAction::SplitBranch: {
      QuestInProgressMap.FindOrAdd(QuestChainData.QuestID, {});
      if (QuestInProgressMap[QuestChainData.QuestID].ChainCompletedIDs.Contains(QuestC->ChainID)) break;

      QuestInProgressMap[QuestChainData.QuestID].ChainCompletedIDs.Add(QuestC->ChainID);

      if (QuestC->QuestOutcomeType == EQuestOutcomeType::DialogueChoice)
        QuestInProgressMap[QuestChainData.QuestID].ChoicesMade.Append(MadeChoiceIds);

      if (QuestC->QuestOutcomeType == EQuestOutcomeType::Negotiation)
        QuestInProgressMap[QuestChainData.QuestID].NegotiationOutcomesMap.Add(QuestC->ChainID, bNegotiationSuccess);
      break;
    }
    case EQuestAction::End: {
      QuestsCompleted.Add(QuestChainData.QuestID);
      if (QuestInProgressMap.Contains(QuestChainData.QuestID)) QuestInProgressMap.Remove(QuestChainData.QuestID);
      break;
    }
    default: checkNoEntry();
  }

  // ? Store npcs with active quests?
  QuestC->PostQuest(GlobalStaticDataManager->GetQuestDialogue(QuestChainData.PostDialogueChainID));
}
