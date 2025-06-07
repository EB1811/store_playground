#include "MenuHeaderWidget.h"
#include "Logging/LogVerbosity.h"
#include "MenuHeaderTabWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UMenuHeaderWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  check(MenuHeaderTabWidgetClass);

  switch (HeaderType) {
    case EHeaderType::Primary: HeaderBorder->SetBrushColor(FColor::FromHex("FFDC6AFF")); break;
    case EHeaderType::Secondary: HeaderBorder->SetBrushColor(FColor::FromHex("F7F7F7FF")); break;
  }
  TitleText->SetText(Title);
}

void UMenuHeaderWidget::SetComponentUI(TArray<FTopBarTab>& TopBarTabs,
                                       std::function<void(FText TabText)> _TabSelectedFunc) {
  check(_TabSelectedFunc);
  check(TopBarTabs.Num() > 0);

  TabSelectedFunc = _TabSelectedFunc;

  ActiveTabWidget = nullptr;
  TopBarWrapBox->ClearChildren();
  for (const FTopBarTab& Tab : TopBarTabs) {
    if (UMenuHeaderTabWidget* MenuHeaderTabWidget =
            CreateWidget<UMenuHeaderTabWidget>(GetWorld(), MenuHeaderTabWidgetClass)) {
      MenuHeaderTabWidget->TabText->SetText(Tab.TabText);
      MenuHeaderTabWidget->OnTabClickedFunc = [this](UMenuHeaderTabWidget* TabWidget) {
        check(TabWidget);

        ActiveTabWidget->SetIsEnabled(true);

        ActiveTabWidget = TabWidget;
        ActiveTabWidget->SetIsEnabled(false);

        TabSelectedFunc(TabWidget->TabText->GetText());
      };

      if (!ActiveTabWidget) {
        ActiveTabWidget = MenuHeaderTabWidget;
        ActiveTabWidget->SetIsEnabled(false);
      }

      TopBarWrapBox->AddChildToWrapBox(MenuHeaderTabWidget);
    } else {
      check(false);
    }
  }
}
