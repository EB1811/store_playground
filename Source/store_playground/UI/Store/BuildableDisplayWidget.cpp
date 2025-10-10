#include "BuildableDisplayWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBuildableDisplayWidget::NativeConstruct() {
  Super::NativeConstruct();

  BuildStockDisplayButton->ControlButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::BuildStockDisplay);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::Back);
}

void UBuildableDisplayWidget::BuildStockDisplay() {
  if (Buildable->BuildingPricesMap[EBuildableType::StockDisplay] > Store->Money) return;

  bool bBuilt = (Store->BuildStockDisplay(Buildable));
  check(bBuilt);

  UGameplayStatics::PlaySound2D(this, BuildSound, 1.0f);

  CloseWidgetFunc();
}

void UBuildableDisplayWidget::Back() { CloseWidgetFunc(); }

void UBuildableDisplayWidget::RefreshUI() {
  check(Buildable && Store);

  float Price = Store->GetBuildablePrice(Buildable);
  float Money = Store->Money;

  PriceText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Price)));
  MoneyNumberText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Money)));

  if (Price > Money) {
    BuildStockDisplayButton->SetActiveStyle(false);
    BuildStockDisplayButton->ControlButton->SetIsEnabled(false);
  } else {
    BuildStockDisplayButton->SetActiveStyle(true);
    BuildStockDisplayButton->ControlButton->SetIsEnabled(true);
  }
}

void UBuildableDisplayWidget::InitUI(FInUIInputActions InUIInputActions,
                                     ABuildable* _Buildable,
                                     AStore* _Store,
                                     std::function<void()> _CloseWidgetFunc) {
  check(_Buildable && _Store && _CloseWidgetFunc);

  Buildable = _Buildable;
  Store = _Store;
  CloseWidgetFunc = _CloseWidgetFunc;

  BuildStockDisplayButton->ActionText->SetText(FText::FromString("Build"));
  BuildStockDisplayButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);
  BackButton->SetActiveStyle(false);
}