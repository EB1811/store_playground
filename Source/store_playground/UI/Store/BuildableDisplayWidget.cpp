#include "BuildableDisplayWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBuildableDisplayWidget::NativeConstruct() {
  Super::NativeConstruct();

  BuildStockDisplayButton->ControlButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::BuildStockDisplay);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::Back);
}

void UBuildableDisplayWidget::BuildStockDisplay() {
  bool bBuilt = (Store->BuildStockDisplay(Buildable));
  check(bBuilt);

  CloseWidgetFunc();
}

void UBuildableDisplayWidget::Back() { CloseWidgetFunc(); }

void UBuildableDisplayWidget::RefreshUI() {
  check(Buildable && Store);

  float Price = Buildable->BuildingPricesMap[EBuildableType::StockDisplay];
  float Money = Store->Money;

  PriceText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Price)));
  MoneyNumberText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Money)));

  if (Price > Money) {
    BuildStockDisplayButton->ControlButton->SetBackgroundColor(FColor::FromHex("F7F7F7FF"));
    BuildStockDisplayButton->ControlButton->SetIsEnabled(false);
  } else {
    BuildStockDisplayButton->ControlButton->SetBackgroundColor(FColor::FromHex("6A8DFFFF"));
    BuildStockDisplayButton->ControlButton->SetIsEnabled(true);
  }
}

void UBuildableDisplayWidget::InitUI(ABuildable* _Buildable, AStore* _Store, std::function<void()> _CloseWidgetFunc) {
  check(_Buildable && _Store && _CloseWidgetFunc);

  Buildable = _Buildable;
  Store = _Store;
  CloseWidgetFunc = _CloseWidgetFunc;

  BuildStockDisplayButton->ActionText->SetText(FText::FromString("Build"));
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->ControlButton->SetBackgroundColor(FColor::FromHex("6A8DFFFF"));
}