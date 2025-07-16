#include "GameOverViewWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Framework/StorePGGameMode.h"

void UGameOverViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  BackToMenuButton->OnClicked.AddDynamic(this, &UGameOverViewWidget::BackToMenu);
}

void UGameOverViewWidget::BackToMenu() {
  AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(GameMode);
  GameMode->ExitToMainMenu();
}

void UGameOverViewWidget::InitUI(FInUIInputActions InUIInputActions) {}
