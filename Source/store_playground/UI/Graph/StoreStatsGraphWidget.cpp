#include "StoreStatsGraphWidget.h"
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

void UStoreStatsGraphWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

int32 UStoreStatsGraphWidget::NativePaint(const FPaintArgs& Args,
                                          const FGeometry& AllottedGeometry,
                                          const FSlateRect& MyCullingRect,
                                          FSlateWindowElementList& OutDrawElements,
                                          int32 LayerId,
                                          const FWidgetStyle& InWidgetStyle,
                                          bool bParentEnabled) const {
  if (Points.Num() < 2) return LayerId;

  // Draw the Zero dashed line.
  if (ZeroLinePoints.Num() >= 2) {
    FLinearColor ZeroLineColor =
        FLinearColor(StoreStatsGraphUIParams.ZeroLineColor.R, StoreStatsGraphUIParams.ZeroLineColor.G,
                     StoreStatsGraphUIParams.ZeroLineColor.B,
                     StoreStatsGraphUIParams.ZeroLineColor.A * InWidgetStyle.GetColorAndOpacityTint().A);

    FSlateDrawElement::MakeDashedLines(OutDrawElements, LayerId + 2, AllottedGeometry.ToPaintGeometry(),
                                       {ZeroLinePoints[0], ZeroLinePoints[1]}, ESlateDrawEffect::None, ZeroLineColor,
                                       StoreStatsGraphUIParams.ZeroLineThickness);
  }

  // Draw the lines.
  FLinearColor LineColor = FLinearColor(StoreStatsGraphUIParams.LineColor.R, StoreStatsGraphUIParams.LineColor.G,
                                        StoreStatsGraphUIParams.LineColor.B,
                                        StoreStatsGraphUIParams.LineColor.A * InWidgetStyle.GetColorAndOpacityTint().A);
  FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points,
                               ESlateDrawEffect::None, LineColor, true, StoreStatsGraphUIParams.LineThickness);
  if (StoreStatsGraphUIParams.bShowPoints) {
    for (const FVector2D& Point : Points) {
      FSlateDrawElement::MakeBox(
          OutDrawElements, LayerId + 1,
          AllottedGeometry.ToPaintGeometry(
              StoreStatsGraphUIParams.PointSize,
              FSlateLayoutTransform(FVector2D(Point.X - StoreStatsGraphUIParams.PointSize.X / 2.0f,
                                              Point.Y - StoreStatsGraphUIParams.PointSize.Y / 2.0f))),
          &StoreStatsGraphUIParams.PointBrush, ESlateDrawEffect::None, LineColor);
    }
  }

  return LayerId + 3;
}

void UStoreStatsGraphWidget::CreateStoreStatsGraph() {
  if (StatsHistory.Num() < 2) {
    Points.Empty();
    return;
  }

  float WidgetWidth = GetDesiredSize().X > 0 ? GetDesiredSize().X : StoreStatsGraphUIParams.XSize;
  float WidgetHeight = GetDesiredSize().Y > 0 ? GetDesiredSize().Y : StoreStatsGraphUIParams.YSize;

  int32 NumPoints = FMath::Min(StoreStatsGraphUIParams.XPoints, StatsHistory.Num() - 1);
  float XPointsScale = WidgetWidth / StoreStatsGraphUIParams.XPoints;

  // Note: Min value can be negative.
  float MaxValue = 0;
  float MinValue = TNumericLimits<float>::Max();
  for (int32 i = 0; i <= NumPoints; ++i) {
    float Value = StatsHistory[StatsHistory.Num() - 1 - i];
    MaxValue = FMath::Max(MaxValue, Value);
    MinValue = FMath::Min(MinValue, Value);
  }
  MaxValue += (StoreStatsGraphUIParams.YScaleMulti * MaxValue) + StoreStatsGraphUIParams.YScaleAddition;
  MaxValue = FMath::Max(MaxValue, StoreStatsGraphUIParams.YScaleMin);
  MinValue -= (StoreStatsGraphUIParams.YScaleMulti * FMath::Abs(MinValue)) + StoreStatsGraphUIParams.YScaleAddition;

  Points.Empty();
  Points.Reserve(NumPoints);
  for (int32 i = 0; i <= NumPoints; ++i) {
    float XValue = (XPointsScale * i);
    float Value = StatsHistory[i];
    float YValue = WidgetHeight - (((Value - MinValue) / (MaxValue - MinValue)) * WidgetHeight);

    Points.Add(FVector2D(XValue, YValue));
  }

  ZeroLinePoints.Empty();
  if (MinValue < 0 && MaxValue > 0) {
    float ZeroPriceY = WidgetHeight - (((0 - MinValue) / (MaxValue - MinValue)) * WidgetHeight);

    ZeroLinePoints.Reserve(2);
    ZeroLinePoints.Add({0, ZeroPriceY});
    ZeroLinePoints.Add({WidgetWidth, ZeroPriceY});
  }
}

void UStoreStatsGraphWidget::RefreshUI() {
  if (StatsHistory.Num() < 2) {
    Points.Empty();
    return;
  }

  CreateStoreStatsGraph();
};

void UStoreStatsGraphWidget::InitUI(const TArray<float> _StatsHistory) {
  StatsHistory = _StatsHistory;

  Points.Empty();
}