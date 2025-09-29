#include "CreditsWidget.h"
#include "CoreFwd.h"
#include "Misc/AssertionMacros.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCreditsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  BackButton->OnClicked.AddDynamic(this, &UCreditsWidget::Back);

  SetupUIActionable();
}

void UCreditsWidget::Back() {
  UGameplayStatics::PlaySound2D(this, BackSound, 1.0f);

  BackFunc();
}

void UCreditsWidget::InitUI(FInUIInputActions _InUIInputActions, std::function<void()> _BackFunc) {
  check(_BackFunc);

  BackFunc = _BackFunc;
}

void UCreditsWidget::SetupUIActionable() {
  UIActionable.RetractUI = [this]() { Back(); };
}