// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuHeaderWidget.generated.h"

UENUM()
enum class EHeaderType : uint8 {
  Primary UMETA(DisplayName = "Primary"),
  Secondary UMETA(DisplayName = "Secondary"),
};

USTRUCT()
struct FTopBarTab {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText TabText;
};

UCLASS()
class STORE_PLAYGROUND_API UMenuHeaderWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UMenuHeaderTabWidget> MenuHeaderTabWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UBorder* HeaderBorder;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* TopBarWrapBox;

  UPROPERTY(EditAnywhere)
  EHeaderType HeaderType;
  UPROPERTY(EditAnywhere)
  FText Title;

  UPROPERTY(EditAnywhere)
  class UMenuHeaderTabWidget* ActiveTabWidget;

  void SetTitle(const FText& NewTitle);

  void SetComponentUI(TArray<FTopBarTab>& TopBarTabs, std::function<void(FText TabText)> _TabSelectedFunc);

  std::function<void(FText TabText)> TabSelectedFunc;
};