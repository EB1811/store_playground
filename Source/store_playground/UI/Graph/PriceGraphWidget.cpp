#include "PriceGraphWidget.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Logging/LogVerbosity.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "Widgets/Notifications/SProgressBar.h"

void UPriceGraphWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

int32 UPriceGraphWidget::NativePaint(const FPaintArgs& Args,
                                     const FGeometry& AllottedGeometry,
                                     const FSlateRect& MyCullingRect,
                                     FSlateWindowElementList& OutDrawElements,
                                     int32 LayerId,
                                     const FWidgetStyle& InWidgetStyle,
                                     bool bParentEnabled) const {
  // ! This runs every frame, might have to change to use UWidget and slate.
  if (!Prices || Prices->Num() < 2) return LayerId;

  float WidgetWidth = AllottedGeometry.GetLocalSize().X;
  float WidgetHeight = AllottedGeometry.GetLocalSize().Y;

  int32 NumPoints = FMath::Min(PriceGraphUIParams.XPoints, Prices->Num() - 1);
  float XPointsScale = WidgetWidth / PriceGraphUIParams.XPoints;

  // Calculate the Y scale based on the max and min prices.
  float MaxPrice = 0;
  float MinPrice = TNumericLimits<float>::Max();
  for (int32 i = 0; i <= NumPoints; ++i) {
    float Price = (*Prices)[Prices->Num() - 1 - i];
    MaxPrice = FMath::Max(MaxPrice, Price);
    MinPrice = FMath::Min(MinPrice, Price);
  }
  MaxPrice += PriceGraphUIParams.YScaleMulti * (MaxPrice - MinPrice);
  MinPrice -= PriceGraphUIParams.YScaleMulti * (MaxPrice - MinPrice);

  // Draw the lines.
  FLinearColor LineColor =
      FLinearColor(PriceGraphUIParams.LineColor.R, PriceGraphUIParams.LineColor.G, PriceGraphUIParams.LineColor.B,
                   PriceGraphUIParams.LineColor.A * InWidgetStyle.GetColorAndOpacityTint().A);
  TArray<FVector2D> Points;
  Points.Reserve(NumPoints);
  for (int32 i = 0; i <= NumPoints; ++i) {
    float XValue = WidgetWidth - (XPointsScale * i);
    float Price = (*Prices)[Prices->Num() - 1 - i];
    float YValue = WidgetHeight - (((Price - MinPrice) / (MaxPrice - MinPrice)) * WidgetHeight);

    Points.Add(FVector2D(XValue, YValue));
  }
  FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points,
                               ESlateDrawEffect::None, LineColor, true, PriceGraphUIParams.LineThickness);
  if (PriceGraphUIParams.bShowPoints) {
    for (const FVector2D& Point : Points) {
      FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 1,
                                 AllottedGeometry.ToPaintGeometry(
                                     PriceGraphUIParams.PointSize,
                                     FSlateLayoutTransform(FVector2D(Point.X - PriceGraphUIParams.PointSize.X / 2.0f,
                                                                     Point.Y - PriceGraphUIParams.PointSize.Y / 2.0f))),
                                 &PriceGraphUIParams.PointBrush, ESlateDrawEffect::None, LineColor);
    }
  }

  // Draw the "Bought At" dashed line.
  if (BoughtAtPrice > 0) {
    FLinearColor BoughtAtColor = FLinearColor(
        PriceGraphUIParams.BoughtAtColor.R, PriceGraphUIParams.BoughtAtColor.G, PriceGraphUIParams.BoughtAtColor.B,
        PriceGraphUIParams.BoughtAtColor.A * InWidgetStyle.GetColorAndOpacityTint().A);

    float ClampedBoughtAtPrice = FMath::Clamp(BoughtAtPrice, MinPrice, MaxPrice);
    float BoughtAtY = WidgetHeight - (((BoughtAtPrice - MinPrice) / (MaxPrice - MinPrice)) * WidgetHeight);

    FVector2f BoughtAtStart(0, BoughtAtY);
    FVector2f BoughtAtEnd(WidgetWidth, BoughtAtY);
    FSlateDrawElement::MakeDashedLines(OutDrawElements, LayerId + 2, AllottedGeometry.ToPaintGeometry(),
                                       {BoughtAtStart, BoughtAtEnd}, ESlateDrawEffect::None, BoughtAtColor,
                                       PriceGraphUIParams.BoughtAtLineThickness);
  }

  return LayerId + 3;
}

void UPriceGraphWidget::InitUI(const TArray<float>* _Prices, float _BoughtAtPrice) {
  Prices = _Prices;
  BoughtAtPrice = _BoughtAtPrice;
}