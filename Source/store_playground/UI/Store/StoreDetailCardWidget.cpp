#include "StoreDetailCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void UStoreDetailCardWidget::InitUI(FText Title,
                                    FText SecondaryTitle,
                                    FText _Text1,
                                    FText _Text2,
                                    FText _SecondaryText) {
  TitleText->SetText(Title);

  if (!SecondaryTitle.IsEmpty()) {
    SecondaryTitleText->SetText(SecondaryTitle);
    SecondaryTitleText->SetVisibility(ESlateVisibility::Visible);
  } else {
    SecondaryTitleText->SetVisibility(ESlateVisibility::Collapsed);
  }
  if (!_Text1.IsEmpty()) {
    Text1->SetText(_Text1);
    Text1->SetVisibility(ESlateVisibility::Visible);
  } else {
    Text1->SetVisibility(ESlateVisibility::Collapsed);
  }
  if (!_Text2.IsEmpty()) {
    Text2->SetText(_Text2);
    Text2->SetVisibility(ESlateVisibility::Visible);
  } else {
    Text2->SetVisibility(ESlateVisibility::Collapsed);
  }
  if (!_SecondaryText.IsEmpty()) {
    SecondaryText->SetText(_SecondaryText);
    SecondaryText->SetVisibility(ESlateVisibility::Visible);
  } else {
    SecondaryText->SetVisibility(ESlateVisibility::Collapsed);
  }
}