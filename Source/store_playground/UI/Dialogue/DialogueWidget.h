// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/UI/UIStructs.h"
#include "DialogueWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDialogueWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UDialogueBoxWidget* DialogueBoxWidget;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* ChoicesPanelWrapBox;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDialogueChoiceWidget> DialogueChoiceWidgetClass;

  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystemRef;

  void InitDialogueUI(class UDialogueSystem* DialogueSystem);
  void UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast = false);

  UFUNCTION()
  void OnNext();
  UFUNCTION()
  void OnChoiceSelect(int32 ChoiceIndex);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;

  std::function<void()> CloseDialogueUI;
};