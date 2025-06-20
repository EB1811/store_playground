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
  class UChoicesBoxWidget* ChoicesBoxWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDialogueChoiceWidget> DialogueChoiceWidgetClass;

  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystem;

  void InitDialogueUI(class UDialogueSystem* _DialogueSystem);
  void UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast = false);

  UFUNCTION()
  void OnNext();
  UFUNCTION()
  void OnChoiceSelect(int32 ChoiceIndex);

  UFUNCTION(BlueprintImplementableEvent)
  void OnVisibilityChangeRequested(ESlateVisibility NewVisibility);

  void InitUI(FInputActions InputActions);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;

  std::function<void()> CloseDialogueUI;
};