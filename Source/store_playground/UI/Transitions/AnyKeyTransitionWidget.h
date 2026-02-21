// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnyKeyTransitionWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAnyKeyTransitionWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UFUNCTION()
  void KeySelected(FInputChord SelectedKey);

  UFUNCTION(BlueprintImplementableEvent)
  void FadeOut();

  UFUNCTION(BlueprintCallable)
  void OnFadeOutEnded();

  std::function<void()> FadeOutEndFunc;
};