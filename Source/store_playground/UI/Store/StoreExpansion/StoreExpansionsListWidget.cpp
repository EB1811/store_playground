#include "StoreExpansionsListWidget.h"
#include "StoreExpansionSelectWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"

void UStoreExpansionsListWidget::RefreshUI() {
  check(StoreExpansionManagerRef && SelectExpansionFunc);

  ExpansionsListPanelWrapBox->ClearChildren();

  for (const auto& Expansion : StoreExpansionManagerRef->StoreExpansions) {
    if (UStoreExpansionSelectWidget* ExpansionWidget =
            CreateWidget<UStoreExpansionSelectWidget>(GetWorld(), StoreExpansionSelectWidgetClass)) {
      ExpansionWidget->SelectExpansionFunc = [this](EStoreExpansionLevel ExpansionLevel) {
        SelectExpansionFunc(ExpansionLevel);
        RefreshUI();
      };
      ExpansionWidget->SetExpansionData(Expansion);
      ExpansionsListPanelWrapBox->AddChildToWrapBox(ExpansionWidget);
    }
  }
}
