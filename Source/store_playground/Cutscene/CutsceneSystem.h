// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "CutsceneSystem.generated.h"

UENUM()
enum class ECutsceneState : uint8 {
  None UMETA(DisplayName = "NONE"),
  InDialogue UMETA(DisplayName = "InDialogue"),
  Waiting UMETA(DisplayName = "Waiting"),
  Finished UMETA(DisplayName = "Finished"),
};
UENUM()
enum class ECutsceneAction : uint8 {
  StartDialogue UMETA(DisplayName = "StartDialogue"),
  StartCutsceneAction UMETA(DisplayName = "StartCutsceneAction"),
  FinishCutscene UMETA(DisplayName = "FinishCutscene"),
};
USTRUCT()
struct FCStateAction {
  GENERATED_BODY()

  ECutsceneState initial;
  ECutsceneAction action;
  ECutsceneState next;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API UCutsceneSystem : public UObject {
  GENERATED_BODY()

public:
  UCutsceneSystem();

  UPROPERTY(EditAnywhere)
  TArray<FCStateAction> StateTransitions;
  ECutsceneState GetNextCutsceneState(ECutsceneState State, ECutsceneAction Action);

  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystem;
  UPROPERTY(EditAnywhere)
  class ACutsceneManager* CutsceneManager;

  UPROPERTY(EditAnywhere)
  ECutsceneState CutsceneState;

  UPROPERTY(EditAnywhere)
  FResolvedCutsceneData ResolvedCutsceneData;
  UPROPERTY(EditAnywhere)
  int32 CurrentCutsceneChainIndex;

  void StartCutscene(const FResolvedCutsceneData& _ResolvedCutsceneData);
  auto NextCutsceneChain() -> ECutsceneState;

  auto PerformCutsceneChainDialogues() -> FNextDialogueRes;
  void PerformCutsceneAction(std::function<void()> ActionFinishedFunc = nullptr);

  void ResetCutscene();
};
