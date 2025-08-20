// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreStatsGraphWidget.generated.h"

// UI params
USTRUCT()
struct FStoreStatsGraphUIParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float XSize;
  UPROPERTY(EditAnywhere)
  float YSize;

  UPROPERTY(EditAnywhere)
  FLinearColor LineColor;
  UPROPERTY(EditAnywhere)
  float LineThickness;

  UPROPERTY(EditAnywhere)
  FLinearColor ZeroLineColor;
  UPROPERTY(EditAnywhere)
  float ZeroLineThickness;

  UPROPERTY(EditAnywhere)
  bool bShowPoints;
  UPROPERTY(EditAnywhere)
  FVector2D PointSize;
  UPROPERTY(EditAnywhere)
  FSlateBrush PointBrush;  // Brush for the points on the graph.

  UPROPERTY(EditAnywhere)
  int32 XPoints;  // Number of points on the X-axis.
  UPROPERTY(EditAnywhere)
  float YScaleMulti;  // Percentage of the max floats to scale the Y-axis.
  UPROPERTY(EditAnywhere)
  float YScaleAddition;  // Additional value to add to the Y-axis scale.
  UPROPERTY(EditAnywhere)
  float YScaleMin;  // Minimum value for the Y-axis scale, if needed.
};

UCLASS()
class STORE_PLAYGROUND_API UStoreStatsGraphWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  virtual int32 NativePaint(const FPaintArgs& Args,
                            const FGeometry& AllottedGeometry,
                            const FSlateRect& MyCullingRect,
                            FSlateWindowElementList& OutDrawElements,
                            int32 LayerId,
                            const FWidgetStyle& InWidgetStyle,
                            bool bParentEnabled) const override;

  UPROPERTY(EditAnywhere)
  FStoreStatsGraphUIParams StoreStatsGraphUIParams;

  UPROPERTY(EditAnywhere)
  TArray<float> StatsHistory;

  UPROPERTY(EditAnywhere)
  TArray<FVector2D> Points;
  UPROPERTY(EditAnywhere)
  TArray<FVector2f> ZeroLinePoints;

  void CreateStoreStatsGraph();

  void RefreshUI();
  void InitUI(const TArray<float> _StatsHistory);
};