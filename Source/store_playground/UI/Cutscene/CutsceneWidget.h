// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/UI/UIStructs.h"
#include "CutsceneWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UCutsceneWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  // Note: Only dialogue widget for now.
  UPROPERTY(meta = (BindWidget))
  class UDialogueWidget* DialogueWidget;

  UPROPERTY(EditAnywhere)
  class UCutsceneSystem* CutsceneSystemRef;

  void InitUI(class UCutsceneSystem* _CutsceneSystemRef);
  void RefreshUI();
  void SkipCutscene();  // * Skips to next cutscene chain, not the whole cutscene.

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;

  std::function<void()> CloseThisUI;
};