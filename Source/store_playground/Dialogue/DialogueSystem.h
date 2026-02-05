// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>
#include <optional>
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataStructs.h"
#include "DialogueSystem.generated.h"

EDialogueState GetNextDialogueState(EDialogueState CurrentState, EDialogueAction Action);
TArray<int32> GetChildChoiceIndexes(const TArray<FDialogueData>& DialogueDataArr,
                                    int32 StartIndex,
                                    int32 ChoicesAmount);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API UDialogueSystem : public UObject {
  GENERATED_BODY()

public:
  UDialogueSystem() : DialogueState(EDialogueState::None), CurrentDialogueIndex(0) {}

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  class UDialogueComponent* DialogueC;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  EDialogueState DialogueState;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  FText SpeakerName;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<FDialogueData> DialogueDataArr;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  int32 CurrentDialogueIndex;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<int32> InquireBlockIndexes;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<FName> ChoiceDialoguesSelectedIDs;

  FNextDialogueRes StartDialogue(class UDialogueComponent* _DialogueC);
  FNextDialogueRes StartDialogue(const TArray<FDialogueData> _DialogueDataArr, const FString& _SpeakerName = "NPC");
  FNextDialogueRes NextDialogue();
  TArray<FDialogueData> GetChoiceDialogues();
  FNextDialogueRes DialogueChoice(int32 ChoiceIndex);
  TArray<FDialogueData> GetInquireDialogues();
  FNextDialogueRes InquireDialogue(int32 InquireIndex);

  void ResetDialogue();
};