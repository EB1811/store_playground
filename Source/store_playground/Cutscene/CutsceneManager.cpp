#include "CutsceneManager.h"
#include "GameplayTagContainer.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Tags/TagsComponent.h"

void ACutsceneManager::BeginPlay() { Super::BeginPlay(); }

void ACutsceneManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

// ? Pass in caller tag to play specific cutscenes. Currently, just using the first tag in the array.
bool ACutsceneManager::PlayPotentialCutscene(const UTagsComponent* PlayerTags) {
  if (PlayerTags->CutsceneTags.IsEmpty()) return false;

  // Just using the first tag.
  FGameplayTag Tag = PlayerTags->CutsceneTags.GetByIndex(0);
  check(CutsceneManagerParams.TagToCutsceneMap.Contains(Tag.GetTagName()));
  const FName CutsceneId = CutsceneManagerParams.TagToCutsceneMap[Tag.GetTagName()];

  FCutsceneData CutsceneData = *GlobalStaticDataManager->CutscenesArray.FindByPredicate(
      [&](const FCutsceneData& Cutscene) { return Cutscene.ID == CutsceneId; });
  TArray<FCutsceneChainData> CutsceneChains = GlobalStaticDataManager->CutsceneChainsArray.FilterByPredicate(
      [&](const FCutsceneChainData& Chain) { return Chain.CutsceneID == CutsceneId; });
  TArray<FDialogueData> DialogueDataArray = {};
  for (const auto& Chain : CutsceneChains) {
    TArray<FDialogueData> DialogueArray = GlobalStaticDataManager->CutsceneDialoguesMap[Chain.RelevantId].Dialogues;
    DialogueDataArray.Append(DialogueArray);
  }

  // * Set up starting NPCs and locations.

  //

  PlayerCommand->CommandCutscene({CutsceneData, CutsceneChains, DialogueDataArray}, Tag);
  return true;
}