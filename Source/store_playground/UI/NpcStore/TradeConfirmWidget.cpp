#include "TradeConfirmWidget.h"
#include "UObject/Class.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UTradeConfirmWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  IncreaseQuantityButton->OnClicked.AddDynamic(this, &UTradeConfirmWidget::IncreaseQuantity);
  DecreaseQuantityButton->OnClicked.AddDynamic(this, &UTradeConfirmWidget::DecreaseQuantity);
  ConfirmTradeButton->ControlButton->OnClicked.AddDynamic(this, &UTradeConfirmWidget::ConfirmTrade);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UTradeConfirmWidget::Back);
}

void UTradeConfirmWidget::ChangeQuantity(float Direction) {
  if (Direction > 0) IncreaseQuantity();
  else if (Direction < 0) DecreaseQuantity();
}

void UTradeConfirmWidget::IncreaseQuantity() {
  switch (TradeType) {
    case ETradeType::Buy:
      if ((Quantity + 1) * ShowPriceFunc() > Money) {
        ErrorText->SetText(FText::FromString("Not enough money!"));
        break;
      }

      Quantity++;
      break;
    case ETradeType::Sell:
      if (Quantity + 1 > AvailableQuantity) {
        ErrorText->SetText(FText::FromString("Not enough items!"));
        break;
      }

      Quantity++;
      break;
    default: checkNoEntry();
  }

  RefreshUI();
}
void UTradeConfirmWidget::DecreaseQuantity() {
  if (Quantity < 1) {
    ErrorText->SetText(FText::FromString("Quantity cannot be less than 0!"));
    return;
  }

  Quantity--;
  RefreshUI();
}

void UTradeConfirmWidget::ConfirmTrade() {
  if (Quantity < 1) return BackFunc();

  ConfirmTradeFunc(Quantity);
}

void UTradeConfirmWidget::Back() { BackFunc(); }

void UTradeConfirmWidget::RefreshUI() {
  float Price = ShowPriceFunc();
  // UE_LOG(LogTemp, Log, TEXT("TradeConfirmWidget::RefreshUI: Price: %.0f, Quantity: %d"), Price, Quantity);

  QuantityText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Quantity)));
  TradePrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Price * Quantity)));
  HaveNumberText->SetText(TradeType == ETradeType::Buy
                              ? FText::FromString(FString::Printf(TEXT("%.0f¬"), Money))
                              : FText::FromString(FString::Printf(TEXT("%d"), AvailableQuantity)));
}

void UTradeConfirmWidget::InitUI(ETradeType _TradeType,
                                 const FText& ItemName,
                                 int32 _AvailableQuantity,
                                 float _Money,
                                 std::function<float()> _ShowPriceFunc,
                                 std::function<void(int32)> _ConfirmTradeFunc,
                                 std::function<void()> _BackFunc) {
  check(_ShowPriceFunc && _ConfirmTradeFunc && _BackFunc);

  TradeType = _TradeType;
  TitleText->SetText(UEnum::GetDisplayValueAsText(_TradeType));
  Name->SetText(ItemName);
  Quantity = _ShowPriceFunc() > _Money ? 0 : 1;
  AvailableQuantity = _AvailableQuantity;
  Money = _Money;
  ShowPriceFunc = _ShowPriceFunc;

  ConfirmTradeFunc = _ConfirmTradeFunc;
  BackFunc = _BackFunc;

  ErrorText->SetText(FText::FromString(""));

  ConfirmTradeButton->ActionText->SetText(FText::FromString(TradeType == ETradeType::Buy ? "Buy" : "Sell"));
  BackButton->ActionText->SetText(FText::FromString("Back"));
}