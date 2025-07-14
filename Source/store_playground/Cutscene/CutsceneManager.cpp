#include "CutsceneManager.h"
#include "GameplayTagContainer.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Tags/TagsComponent.h"

void ACutsceneManager::BeginPlay() { Super::BeginPlay(); }

void ACutsceneManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

bool ACutsceneManager::PlayPotentialCutscene(FGameplayTagContainer& CallerCutsceneTags) {
  TArray<FCutsceneData> EligibleCutscenes =
      GlobalStaticDataManager->CutscenesArray.FilterByPredicate([this, CallerCutsceneTags](const auto& Cutscene) {
        return !PlayedCutscenes.Contains(Cutscene.ID) && Cutscene.Tags.HasAny(CallerCutsceneTags) &&
               EvaluatePlayerTagsRequirements(Cutscene.PlayerTagsRequirements, PlayerTags);
      });
  if (EligibleCutscenes.Num() <= 0) return false;

  EligibleCutscenes.Sort([](const FCutsceneData& A, const FCutsceneData& B) { return A.Priority < B.Priority; });
  FCutsceneData CutsceneData = EligibleCutscenes[0];

  TArray<FCutsceneChainData> CutsceneChains = GlobalStaticDataManager->CutsceneChainsArray.FilterByPredicate(
      [CutsceneData](const FCutsceneChainData& Chain) { return Chain.CutsceneID == CutsceneData.ID; });

  TArray<FDialogueData> DialogueDataArray = {};
  for (const auto& Chain : CutsceneChains) {
    if (Chain.CutsceneChainType != ECutsceneChainType::Dialogue) continue;

    TArray<FDialogueData> DialogueArray = GlobalStaticDataManager->CutsceneDialoguesMap[Chain.RelevantId].Dialogues;
    DialogueDataArray.Append(DialogueArray);
  }

  // * Set up starting NPCs and locations.

  //

  PlayedCutscenes.Add(CutsceneData.ID);
  PlayerCommand->CommandCutscene({CutsceneData, CutsceneChains, DialogueDataArray});
  return true;
}