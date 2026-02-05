// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstance.h"
#include "store_playground/UI/UIStructs.h"
#include "DialogueWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDialogueWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDialogueChoiceWidget> DialogueChoiceWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UDialogueBoxWidget* DialogueBoxWidget;
  UPROPERTY(meta = (BindWidget))
  class UChoicesBoxWidget* ChoicesBoxWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* OpenSound;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* NextSound;

  // ! Temp using speaker name to avoid refactoring Dialogue struct.
  UPROPERTY(EditAnywhere)
  TMap<FName, UMaterialInstance*> SpeakerMaterialMap;
  UPROPERTY(EditAnywhere)
  UMaterialInstance* DefaultSpeakerMaterial;
  UPROPERTY(EditAnywhere)
  UMaterialInstance* PlayerSpeakerMaterial;

  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystem;

  void UpdateDialogueText(const FText& SpeakerName, const FText& NewDialogueContent, bool IsLast = false);
  void SetDialogueSpeakerMaterial(FName SpeakerID);
  void UpdateDialogueBasedOnState();

  UFUNCTION()
  void Next();
  UFUNCTION()
  void SelectChoice(int32 ChoiceIndex);
  UFUNCTION()
  void SelectInquire(int32 InquireIndex);

  void InitUI();
  void InitUI(FInUIInputActions InputActions,
              class UDialogueSystem* _DialogueSystem,
              std::function<void(bool)> _CloseDialogueFunc);
  void InitUI(FInCutsceneInputActions InputActions,
              class UDialogueSystem* _DialogueSystem,
              std::function<void(bool)> _CloseDialogueFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  // * Passes in whether the dialogue is finished or not.
  std::function<void(bool)> CloseDialogueFunc;
};