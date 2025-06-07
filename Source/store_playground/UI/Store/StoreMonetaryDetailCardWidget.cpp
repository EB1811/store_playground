#include "StoreMonetaryDetailCardWidget.h"
#include "store_playground/UI/Store/MonetaryDetailTextWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UStoreMonetaryDetailCardWidget::InitUI(FText Title, TArray<FMonetaryDetail> MonetaryDetails) {
  TitleText->SetText(Title);

  MonetaryDetailsPanelVerticalBox->ClearChildren();
  for (const FMonetaryDetail& Detail : MonetaryDetails) {
    UMonetaryDetailTextWidget* DetailWidget =
        CreateWidget<UMonetaryDetailTextWidget>(this, MonetaryDetailTextWidgetClass);
    check(DetailWidget);

    DetailWidget->NameText->SetText(Detail.Text);
    DetailWidget->ValueText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Detail.Value)));
    MonetaryDetailsPanelVerticalBox->AddChild(DetailWidget);
  }
}