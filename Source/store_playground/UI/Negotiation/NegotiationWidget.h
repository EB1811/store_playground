// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NegotiationWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UNegotiationElementWidget* NegotiationElementWidget;

  UPROPERTY(meta = (BindWidget))
  class UDialogueWidget* DialogueWidget;
  void SetAndOpenDialogue(class UDialogueSystem* Dialogue);

  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystemRef;

  void InitNegotiationUI();
  void RefreshNegotiationState();

  UFUNCTION()
  void OnReadDialogueButtonClicked();
  UFUNCTION()
  void OnOfferButtonClicked();
  UFUNCTION()
  void OnAcceptButtonClicked();
  UFUNCTION()
  void OnRejectButtonClicked();

  std::function<void()> CloseNegotiationUI;
  std::function<void()> RefreshInventoryUI;
};