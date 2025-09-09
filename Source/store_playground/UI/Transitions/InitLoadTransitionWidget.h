// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InitLoadTransitionWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInitLoadTransitionWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintImplementableEvent)
  void FadeOut();
  UFUNCTION(BlueprintImplementableEvent)
  void NewGameFadeOut();

  UFUNCTION(BlueprintCallable)
  void OnFadeOutEnded();

  std::function<void()> FadeOutEndFunc;
};