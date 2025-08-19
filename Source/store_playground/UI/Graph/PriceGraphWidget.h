// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PriceGraphWidget.generated.h"

// Starting from the right side of the widget.

// UI params
USTRUCT()
struct FPriceGraphUIParams {
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
  FLinearColor BoughtAtColor;
  UPROPERTY(EditAnywhere)
  float BoughtAtLineThickness;

  UPROPERTY(EditAnywhere)
  bool bShowPoints;
  UPROPERTY(EditAnywhere)
  FVector2D PointSize;
  UPROPERTY(EditAnywhere)
  FSlateBrush PointBrush;  // Brush for the points on the graph.

  UPROPERTY(EditAnywhere)
  int32 XPoints;  // Number of points on the X-axis.
  UPROPERTY(EditAnywhere)
  float YScaleMulti;  // Percentage of the max and min prices to scale the Y-axis.
  UPROPERTY(EditAnywhere)
  float YScaleAddition;  // Additional value to add to the Y-axis scale so small prices are squashed.
};

UCLASS()
class STORE_PLAYGROUND_API UPriceGraphWidget : public UUserWidget {
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
  FPriceGraphUIParams PriceGraphUIParams;

  const TArray<float>* Prices;
  UPROPERTY(EditAnywhere)
  float BoughtAtPrice;

  UPROPERTY(EditAnywhere)
  TArray<FVector2D> Points;
  UPROPERTY(EditAnywhere)
  TArray<FVector2f> BoughtAtPoints;

  void CreatePriceGraph();

  void RefreshUI();
  void InitUI(const TArray<float>* _Prices, float _BoughtAtPrice = 0);
};