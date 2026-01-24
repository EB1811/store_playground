#include "MainMenuControlHUD.h"
#include "Components/Button.h"
#include "Components/SlateWrapperTypes.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "store_playground/SaveManager/SettingsSaveGame.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/UI/MainMenu/MainMenuWidget.h"
#include "store_playground/UI/SaveSlots/LoadSlotsWidget.h"
#include "store_playground/UI/Transitions/InitLoadTransitionWidget.h"
#include "store_playground/UI/Transitions/LevelLoadingTransitionWidget.h"

AMainMenuControlHUD::AMainMenuControlHUD() { PrimaryActorTick.bCanEverTick = false; }

void AMainMenuControlHUD::BeginPlay() {
  Super::BeginPlay();

  LevelLoadingTransitionWidget =
      CreateWidget<ULevelLoadingTransitionWidget>(GetWorld(), LevelLoadingTransitionWidgetClass);

  InitLoadTransitionWidget = CreateWidget<UInitLoadTransitionWidget>(GetWorld(), InitLoadTransitionWidgetClass);
  InitLoadTransitionWidget->AddToViewport(100);
  InitLoadTransitionWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);
}

void AMainMenuControlHUD::AdvanceUI() {
  if (!MainMenuWidget) return;

  MainMenuWidget->UIActionable.AdvanceUI();
}
void AMainMenuControlHUD::RetractUIAction() {
  if (!MainMenuWidget) return;

  MainMenuWidget->UIActionable.RetractUI();
}
void AMainMenuControlHUD::UIDirectionalInputAction(FVector2D Direction) {
  if (!MainMenuWidget) return;

  MainMenuWidget->UIActionable.DirectionalInput(Direction);
}
void AMainMenuControlHUD::UISideButton4Action() {
  if (!MainMenuWidget) return;

  MainMenuWidget->UIActionable.SideButton4();
}

void AMainMenuControlHUD::OpenMainMenu() {
  MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
  MainMenuWidget->AddToViewport(20);

  MainMenuWidget->InitUI(this, InUIInputActions, SettingsManager, SaveManager);
  MainMenuWidget->RefreshUI();

  InitMenuFadeOut([this]() {
    MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
    MainMenuWidget->SetFocus();
    MainMenuWidget->ShowSplashScreen();
  });
}

void AMainMenuControlHUD::InitMenuFadeOut(std::function<void()> _FadeOutEndFunc) {
  check(InitLoadTransitionWidget);

  InitLoadTransitionWidget->FadeOutEndFunc = [this, _FadeOutEndFunc]() {
    InitLoadTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    InitLoadTransitionWidget->RemoveFromParent();
    InitLoadTransitionWidget->SetRenderOpacity(1.0f);

    _FadeOutEndFunc();
  };
  InitLoadTransitionWidget->StartupFadeOut();
}

void AMainMenuControlHUD::StartLevelLoadingTransition(std::function<void()> _FadeInEndFunc) {
  LevelLoadingTransitionWidget->FadeInEndFunc = _FadeInEndFunc;

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMainMenuControlHUD::PlayWidgetAnim(class UUserWidget* Widget, class UWidgetAnimation* Animation) {
  if (!Widget || !Animation) return;

  Widget->PlayAnimation(Animation);
}