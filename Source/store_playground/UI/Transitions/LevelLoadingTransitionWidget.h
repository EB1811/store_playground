// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelLoadingTransitionWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ULevelLoadingTransitionWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintImplementableEvent)
  void OnEndLevelLoadingCalled();

  UFUNCTION(BlueprintCallable)
  void OnFadeInEnded();
  UFUNCTION(BlueprintCallable)
  void OnFadeOutEnded();

  std::function<void()> FadeInEndFunc;
  std::function<void()> FadeOutEndFunc;
};