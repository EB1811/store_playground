#include "MenuHeaderTabWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UMenuHeaderTabWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  TabButton->OnClicked.AddDynamic(this, &UMenuHeaderTabWidget::OnTabClicked);
}

void UMenuHeaderTabWidget::OnTabClicked() {
  check(OnTabClickedFunc);

  OnTabClickedFunc(TabIndex);
}