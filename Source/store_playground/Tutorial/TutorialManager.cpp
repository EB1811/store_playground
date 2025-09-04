#include "TutorialManager.h"
#include "GameplayTagContainer.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Tags/TagsComponent.h"

void ATutorialManager::BeginPlay() {
  Super::BeginPlay();

  TutorialSteps.Empty();
  TArray<FUITutorialStepRow*> UITutorialStepRows;
  UITutorialStepTable->GetAllRows<FUITutorialStepRow>("", UITutorialStepRows);
  for (auto Row : UITutorialStepRows)
    TutorialSteps.Add({Row->ID, Row->TutorialID, Row->Title, Row->Body, Row->Image, Row->Video});

  TutorialsData.Empty();
  TArray<FUITutorialDataRow*> UITutorialDataRows;
  UITutorialDataTable->GetAllRows<FUITutorialDataRow>("", UITutorialDataRows);
  for (auto Row : UITutorialDataRows) TutorialsData.Add(Row->IdTag, {Row->ID, Row->IdTag, Row->PlayerTags});

  check(TutorialSteps.Num() > 0);
  check(TutorialsData.Num() > 0);

  UITutorialStepTable = nullptr;
  UITutorialDataTable = nullptr;
}

void ATutorialManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto ATutorialManager::CheckTutorial(FGameplayTag TutorialIdTag) const -> bool {
  if (!TutorialsData.Contains(TutorialIdTag)) {
    UE_LOG(LogTemp, Error, TEXT("TutorialManager: Tag %s called, but not found in TutorialsData."),
           *TutorialIdTag.ToString());
    return false;
  }

  const FUITutorialData& TutorialData = TutorialsData[TutorialIdTag];
  if (!SettingsManager->GameSettings.bShowTutorials || !PlayerTags->ConfigurationTags.HasAll(TutorialData.PlayerTags) ||
      CompletedTutorials.Contains(TutorialData.ID))
    return false;

  return true;
}

auto ATutorialManager::CheckAndShowTutorial(FGameplayTag TutorialIdTag) -> bool {
  if (!CheckTutorial(TutorialIdTag)) return false;

  const FUITutorialData& TutorialData = TutorialsData[TutorialIdTag];
  const TArray<FUITutorialStep> Steps = TutorialSteps.FilterByPredicate(
      [TutorialData](const FUITutorialStep& Step) { return Step.TutorialID == TutorialData.ID; });
  if (Steps.Num() <= 0) {
    UE_LOG(LogTemp, Error, TEXT("TutorialManager: No steps found for tutorial id %s."), *TutorialData.ID.ToString());
    return false;
  }

  if (!PlayerCommand->CommandTutorial(Steps)) return false;

  CompletedTutorials.Add(TutorialData.ID);
  if (PlayerTags->ConfigurationTags.HasTagExact(TutorialIdTag)) PlayerTags->ConfigurationTags.RemoveTag(TutorialIdTag);
  return true;
}

auto ATutorialManager::ShowPendingTutorials() -> bool {
  FGameplayTagContainer TutorialTags = PlayerTags->ConfigurationTags.Filter(StringTagsToContainer({"Tutorial"}));
  if (TutorialTags.IsEmpty()) return false;

  FGameplayTag Tag = FGameplayTag::EmptyTag;
  for (const auto& TutTag : TutorialTags) {
    if (CheckTutorial(TutTag)) {
      Tag = TutTag;
      break;
    }
  }
  if (!Tag.IsValid()) return false;

  return CheckAndShowTutorial(Tag);
}