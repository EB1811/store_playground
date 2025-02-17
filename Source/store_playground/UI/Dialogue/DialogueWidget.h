// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDialogueWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Speaker;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DialogueText;
  UPROPERTY(meta = (BindWidget))
  class UButton* NextButton;

  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystemRef;

  void RefreshDialogueWhole();
  void UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent);

  UFUNCTION()
  void OnNextButtonClicked();

  std::function<void()> CloseDialogueUI;
};