// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "Blueprint/UserWidget.h"
#include "ChoicesBoxWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UChoicesBoxWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDialogueChoiceWidget> DialogueChoiceWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Speaker;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* ChoicesBox;

  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> Dialogues;

  void RefreshUI();
  void InitUI(TArray<FDialogueData>& _Dialogues,
              const FText& SpeakerName,
              std::function<void(int32)> _ChoiceSelectedFunc);

  std::function<void(int32)> ChoiceSelectedFunc;
};