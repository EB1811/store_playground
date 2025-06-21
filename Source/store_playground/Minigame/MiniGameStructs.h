// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "PaperFlipbook.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "MiniGameStructs.generated.h"

UENUM()
enum class EMiniGame : uint8 {
  Lootbox UMETA(DisplayName = "Lootbox"),
};
ENUM_RANGE_BY_COUNT(EMiniGame, 1);

USTRUCT()
struct FMiniGameAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  UStaticMesh* Mesh;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> Sprites;
};

USTRUCT()
struct FMiniGameInfo {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EMiniGame MiniGame;

  UPROPERTY(EditAnywhere)
  float SpawnChance;

  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> TutDialogues;  // * Tutorial dialogue.
  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> InitDialogues;  // * Dialogue before starting.

  UPROPERTY(EditAnywhere)
  FText DisplayName;  // * Name of the minigame, displayed in the UI.
  UPROPERTY(EditAnywhere)
  FMiniGameAssetData AssetData;
};

struct MiniGameT {
  template <class T>
    requires(std::is_pointer_v<T>)
  MiniGameT(T& t)
      : InitMiniGame{[&t](const class AMarket* Market,
                          class AStore* Store,
                          class UInventoryComponent* PlayerInv,
                          TMap<FName, float> PersistentData,
                          std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc,
                          std::function<void(class UUserWidget*)> CloseMinigameFunc) {
          return t->InitMiniGame(Market, Store, PlayerInv, PersistentData, UpdatePersistentDataFunc, CloseMinigameFunc);
        }} {}

  std::function<void(const class AMarket* Market,
                     class AStore* Store,
                     class UInventoryComponent* PlayerInv,
                     TMap<FName, float> PersistentData,
                     std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc,
                     std::function<void(class UUserWidget*)> CloseMinigameFunc)>
      InitMiniGame;
};

// UINTERFACE(MinimalAPI)
// class UMiniGameInterface : public UInterface {
//   GENERATED_BODY()
// };

// class STORE_PLAYGROUND_API IMiniGameInterface {
//   GENERATED_BODY()

// public:
//   virtual void InitMiniGame(const class AMarket* Market,
//                             class AStore* Store,
//                             class UInventoryComponent* PlayerInv,
//                             TMap<FName, float> PersistentData,
//                             std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc,
//                             std::function<void()> CloseWidgetFunc) = 0;
// };