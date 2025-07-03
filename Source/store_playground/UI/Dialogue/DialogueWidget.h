// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/UI/UIStructs.h"
#include "DialogueWidget.generated.h"

// TODO: Separate into view.

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

  void UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast = false);

  UFUNCTION()
  void Next();
  UFUNCTION()
  void SelectChoice(int32 ChoiceIndex);

  void InitUI(FInUIInputActions InUIInputActions,
              class UDialogueSystem* _DialogueSystem,
              std::function<void()> _CloseDialogueFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();
  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  UFUNCTION(BlueprintImplementableEvent)
  void OnVisibilityChangeRequested(ESlateVisibility NewVisibility);

  std::function<void()> CloseDialogueFunc;
};