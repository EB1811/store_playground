// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueChoiceWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDialogueChoiceWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ChoiceText;
  UPROPERTY(meta = (BindWidget))
  class UButton* ChoiceButton;

  UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
  int32 ChoiceIndex;

  UFUNCTION()
  void ClickChoice();

  void InitUI(const FString& ChoiceTextContent, int32 Index, std::function<void(int32)> _ChoiceSelectedFunc);

  std::function<void(int32)> ChoiceSelectedFunc;
};