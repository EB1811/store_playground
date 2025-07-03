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

  // switch (HeaderType) {
  //   case EHeaderType::Primary: HeaderBorder->SetBrushColor(FColor::FromHex("FFDC6AFF")); break;
  //   case EHeaderType::Secondary: HeaderBorder->SetBrushColor(FColor::FromHex("F7F7F7FF")); break;
  // }
  TitleText->SetText(Title);
}

void UMenuHeaderWidget::SetTitle(const FText& NewTitle) {
  Title = NewTitle;
  TitleText->SetText(NewTitle);
}
void UMenuHeaderWidget::SelectTab(int32 TabIndex) {
  check(TabIndex >= 0 && TabIndex < TabWidgets.Num());
  if (ActiveTabIndex == TabIndex) return;

  TabWidgets[ActiveTabIndex]->BottomBorder->SetVisibility(ESlateVisibility::Hidden);
  ActiveTabIndex = TabIndex;
  TabWidgets[ActiveTabIndex]->BottomBorder->SetVisibility(ESlateVisibility::Visible);

  TabSelectedFunc(TabWidgets[ActiveTabIndex]->TabText->GetText());
}
void UMenuHeaderWidget::CycleLeft() {
  if (TabWidgets.Num() <= 1) return;

  int32 NewTabIndex = (ActiveTabIndex - 1 + TabWidgets.Num()) % TabWidgets.Num();
  SelectTab(NewTabIndex);
}
void UMenuHeaderWidget::CycleRight() {
  if (TabWidgets.Num() <= 1) return;

  int32 NewTabIndex = (ActiveTabIndex + 1) % TabWidgets.Num();
  SelectTab(NewTabIndex);
}

void UMenuHeaderWidget::SetComponentUI(TArray<FTopBarTab>& TopBarTabs,
                                       std::function<void(FText TabText)> _TabSelectedFunc,
                                       int32 InitTab) {
  check(TopBarTabs.Num() > 0 && _TabSelectedFunc && InitTab >= 0 && InitTab < TopBarTabs.Num());

  TabSelectedFunc = _TabSelectedFunc;

  TabWidgets.Empty();
  TopBarWrapBox->ClearChildren();
  for (int i = 0; i < TopBarTabs.Num(); i++) {
    const FTopBarTab& Tab = TopBarTabs[i];

    UMenuHeaderTabWidget* MenuHeaderTabWidget =
        CreateWidget<UMenuHeaderTabWidget>(GetWorld(), MenuHeaderTabWidgetClass);
    MenuHeaderTabWidget->TabText->SetText(Tab.TabText);
    MenuHeaderTabWidget->TabIndex = TabWidgets.Num();
    MenuHeaderTabWidget->OnTabClickedFunc = [this](int32 TabIndex) { SelectTab(TabIndex); };

    TabWidgets.Add(MenuHeaderTabWidget);
    TopBarWrapBox->AddChildToWrapBox(MenuHeaderTabWidget);
  }

  ActiveTabIndex = InitTab;
  TabWidgets[ActiveTabIndex]->BottomBorder->SetVisibility(ESlateVisibility::Visible);
}
