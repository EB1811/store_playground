#include "TutorialManager.h"
#include "GameplayTagContainer.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Tags/TagsComponent.h"

void ATutorialManager::BeginPlay() { Super::BeginPlay(); }

void ATutorialManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto ATutorialManager::CheckAndShowTutorial(FGameplayTag TutorialIdTag) -> bool {
  if (!TutorialsData.Contains(TutorialIdTag)) {
    UE_LOG(LogTemp, Error, TEXT("TutorialManager: Tag %s called, but not found in TutorialsData."),
           *TutorialIdTag.ToString());
    return false;
  }

  const FUITutorialData TutorialData = TutorialsData[TutorialIdTag];
  if (!SettingsManager->GameSettings.bShowTutorials || !PlayerTags->ConfigurationTags.HasAll(TutorialData.PlayerTags) ||
      CompletedTutorials.Contains(TutorialData.ID))
    return false;

  const TArray<FUITutorialStep> Steps = TutorialSteps.FilterByPredicate(
      [TutorialData](const FUITutorialStep& Step) { return Step.TutorialID == TutorialData.ID; });
  if (Steps.Num() <= 0) {
    UE_LOG(LogTemp, Error, TEXT("TutorialManager: No steps found for tutorial id %s."), *TutorialData.ID.ToString());
    return false;
  }

  if (!PlayerCommand->CommandTutorial(Steps)) return false;

  CompletedTutorials.Add(TutorialData.ID);
  return true;
}