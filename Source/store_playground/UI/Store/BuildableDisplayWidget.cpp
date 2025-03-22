#include "BuildableDisplayWidget.h"
#include "store_playground/WorldObject/Buildable.h"
#include "Components/Button.h"

void UBuildableDisplayWidget::NativeConstruct() {
  Super::NativeConstruct();

  BuildStockDisplayButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::OnStockDisplayButtonClicked);
  BuildDecorationButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::OnDecorationButtonClicked);
  CancelButton->OnClicked.AddDynamic(this, &UBuildableDisplayWidget::OnCancelButtonClicked);
}

void UBuildableDisplayWidget::SetBuildable(ABuildable* Buildable) {
  check(Buildable);
  BuildableRef = Buildable;

  if (BuildableRef->IsBuildableMap[EBuildableType::StockDisplay])
    BuildStockDisplayButton->SetVisibility(ESlateVisibility::Visible);
  else
    BuildStockDisplayButton->SetVisibility(ESlateVisibility::Collapsed);

  if (BuildableRef->IsBuildableMap[EBuildableType::Decoration])
    BuildDecorationButton->SetVisibility(ESlateVisibility::Visible);
  else
    BuildDecorationButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UBuildableDisplayWidget::OnStockDisplayButtonClicked() {
  check(BuildStockDisplayFunc);

  if (BuildStockDisplayFunc(const_cast<ABuildable*>(BuildableRef)))
    if (CloseWidgetFunc) CloseWidgetFunc();
}

void UBuildableDisplayWidget::OnDecorationButtonClicked() {
  if (BuildDecorationFunc(const_cast<ABuildable*>(BuildableRef)))
    if (CloseWidgetFunc) CloseWidgetFunc();
}

void UBuildableDisplayWidget::OnCancelButtonClicked() {
  if (CloseWidgetFunc) CloseWidgetFunc();
}
