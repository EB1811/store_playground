// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "DebtWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDebtWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DebtTakenText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CurrentOwnedDebtText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* InterestRateText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* MaxDebtText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DebtAmountText;
  UPROPERTY(meta = (BindWidget))
  class USlider* DebtAmountSlider;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PayableInterestText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TotalPayableText;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* IssueDebtButton;

  UPROPERTY(EditAnywhere)
  class USoundBase* ChangeAmountSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* IssueDebtSound;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;

  UPROPERTY(EditAnywhere)
  class ADebtManager* DebtManager;

  UFUNCTION()
  void UpdateDebtTexts(float Value);

  void ChangeAmount(FVector2D Direction);

  UFUNCTION()
  void IssueDebt();

  void ResetAmount();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions, const class AStore* _Store, class ADebtManager* _DebtManager);
};