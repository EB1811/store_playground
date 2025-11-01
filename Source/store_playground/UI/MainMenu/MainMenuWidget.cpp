#include "MainMenuWidget.h"
#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "store_playground/UI/MainMenu/NewGameSetupWidget.h"
#include "store_playground/UI/SaveSlots/SaveLoadSlotsWidget.h"
#include "store_playground/UI/Settings/SettingsWidget.h"
#include "store_playground/UI/MainMenu/CreditsWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "store_playground/MainMenuControl/MainMenuControlHUD.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Continue);
  ContinueButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::NewGame);
  NewGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  LoadMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoadMenu);
  LoadMenuButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::SettingsMenu);
  SettingsButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  CreditsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Credits);
  CreditsButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Exit);
  ExitButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);

  SetupUIActionable();
}

void UMainMenuWidget::SelectHoveredButton() {
  if (!HoveredButton) return;

  if (HoveredButton == ContinueButton) Continue();
  else if (HoveredButton == NewGameButton) NewGame();
  else if (HoveredButton == LoadMenuButton) LoadMenu();
  else if (HoveredButton == SettingsButton) SettingsMenu();
  else if (HoveredButton == CreditsButton) Credits();
  else if (HoveredButton == ExitButton) Exit();
}
void UMainMenuWidget::HoverButton(UButton* Button) {
  check(Button);
  if (HoveredButton == Button) return;

  if (HoveredButton) {
    FButtonStyle ButtonStyle = Button->GetStyle();
    ButtonStyle.SetNormal(ButtonStyle.Normal);
    HoveredButton->SetStyle(ButtonStyle);
  }
  HoveredButton = Button;
  FButtonStyle ButtonStyle = HoveredButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Hovered);
  HoveredButton->SetStyle(ButtonStyle);
  HoveredButton->SetFocus();

  UGameplayStatics::PlaySound2D(this, HoverSound, 1.0f);
}
void UMainMenuWidget::HoverNextButton(FVector2D Direction) {
  if (Direction.X == 0) return;

  if (!HoveredButton) {
    HoverButton(NewGameButton);
    return;
  }

  TArray<UButton*> Buttons = {ContinueButton, NewGameButton, LoadMenuButton, SettingsButton, CreditsButton, ExitButton};
  int32 CurrentIndex = Buttons.IndexOfByKey(HoveredButton);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= Buttons.Num()) return;
  if (!Buttons[NextIndex]->GetIsEnabled()) return;

  HoverButton(Buttons[NextIndex]);
}
void UMainMenuWidget::UnhoverButton() {
  if (!HoveredButton) return;

  FButtonStyle ButtonStyle = HoveredButton == NewGameButton ? LoadMenuButton->GetStyle() : NewGameButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Normal);
  HoveredButton->SetStyle(ButtonStyle);
  HoveredButton = nullptr;
}

void UMainMenuWidget::Continue() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == 0 &&
      SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() <= 0)
    return;

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);

  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);
  MainMenuGameMode->Continue();
}
void UMainMenuWidget::NewGame() {
  auto BackFunc = [this]() {
    this->PlayAnimationReverse(ShowPageAnim, 4.0f);
    MenusOverlay->SetVisibility(ESlateVisibility::Collapsed);
    NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);

    ShowSplashScreen();
    HoverButton(NewGameButton);
  };
  NewGameSetupWidget->InitUI(InUIInputActions, SettingsManager, BackFunc);
  NewGameSetupWidget->RefreshUI();

  this->PlayAnimationReverse(StartupAnim, 4.0f);
  MenusOverlay->SetVisibility(ESlateVisibility::Visible);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Visible);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
  CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);
  MainMenuControlHUD->PlayWidgetAnim(this, ShowPageAnim);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::LoadMenu() {
  auto BackFunc = [this]() {
    this->PlayAnimationReverse(ShowPageAnim, 4.0f);
    MenusOverlay->SetVisibility(ESlateVisibility::Collapsed);
    SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);

    ShowSplashScreen();
    HoverButton(LoadMenuButton);
  };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, false, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  this->PlayAnimationReverse(StartupAnim, 4.0f);
  MenusOverlay->SetVisibility(ESlateVisibility::Visible);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
  CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);
  MainMenuControlHUD->PlayWidgetAnim(this, ShowPageAnim);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::SettingsMenu() {
  auto BackFunc = [this]() {
    this->PlayAnimationReverse(ShowPageAnim, 4.0f);
    MenusOverlay->SetVisibility(ESlateVisibility::Collapsed);
    SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

    ShowSplashScreen();
    HoverButton(SettingsButton);
  };
  SettingsWidget->InitUI(InUIInputActions, SettingsManager, BackFunc);

  this->PlayAnimationReverse(StartupAnim, 4.0f);
  MenusOverlay->SetVisibility(ESlateVisibility::Visible);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Visible);
  CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);
  MainMenuControlHUD->PlayWidgetAnim(this, ShowPageAnim);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::Credits() {
  auto BackFunc = [this]() {
    this->PlayAnimationReverse(ShowPageAnim, 4.0f);
    MenusOverlay->SetVisibility(ESlateVisibility::Collapsed);
    CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);

    ShowSplashScreen();
    HoverButton(CreditsButton);
  };
  CreditsWidget->InitUI(InUIInputActions, BackFunc);

  this->PlayAnimationReverse(StartupAnim, 4.0f);
  MenusOverlay->SetVisibility(ESlateVisibility::Visible);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
  CreditsWidget->SetVisibility(ESlateVisibility::Visible);
  MainMenuControlHUD->PlayWidgetAnim(this, ShowPageAnim);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::Exit() {
  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);

  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);
  MainMenuGameMode->Exit();
}

void UMainMenuWidget::ShowSplashScreen() {
  MainMenuControlHUD->PlayWidgetAnim(this, StartupAnim);

  UGameplayStatics::PlaySound2D(this, StartupSound, 1.0f);
}

void UMainMenuWidget::RefreshUI() {
  if (SaveManager->SaveSlotListSaveGame->bHasAutoSave ||
      (SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() > 0 &&
       SaveManager->SaveSlotListSaveGame->SaveSlotList[SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex]
           .bIsPopulated))
    ContinueButton->SetIsEnabled(true);
  else ContinueButton->SetIsEnabled(false);
}

void UMainMenuWidget::InitUI(AMainMenuControlHUD* _MainMenuControlHUD,
                             FInUIInputActions _InUIInputActions,
                             class ASettingsManager* _SettingsManager,
                             class ASaveManager* _SaveManager) {
  check(_MainMenuControlHUD && _SettingsManager && _SaveManager);

  MainMenuControlHUD = _MainMenuControlHUD;
  InUIInputActions = _InUIInputActions;
  SettingsManager = _SettingsManager;
  SaveManager = _SaveManager;

  MenusOverlay->SetVisibility(ESlateVisibility::Collapsed);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
  CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);

  // HoverButton(NewGameButton);
}

void UMainMenuWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.AdvanceUI();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.AdvanceUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.AdvanceUI();
    else if (CreditsWidget->IsVisible()) CreditsWidget->UIActionable.AdvanceUI();
    else SelectHoveredButton();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.DirectionalInput(Direction);
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.DirectionalInput(Direction);
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.DirectionalInput(Direction);
    else if (CreditsWidget->IsVisible()) CreditsWidget->UIActionable.DirectionalInput(Direction);
    else HoverNextButton(Direction);
  };
  UIActionable.SideButton4 = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.SideButton4();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.SideButton4();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.SideButton4();
    else if (CreditsWidget->IsVisible()) CreditsWidget->UIActionable.SideButton4();
  };
  UIActionable.RetractUI = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.RetractUI();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.RetractUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.RetractUI();
    else if (CreditsWidget->IsVisible()) CreditsWidget->UIActionable.RetractUI();
  };
}