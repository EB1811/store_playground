// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/SlateEnums.h"
#include "UpgradeStructs.generated.h"

UENUM()
enum class EUpgradeClass : uint8 {
  Holy UMETA(DisplayName = "Holy"),
  Demonic UMETA(DisplayName = "Demonic"),
  Artisanal UMETA(DisplayName = "Artisanal"),
  Eldritch UMETA(DisplayName = "Eldritch"),
};

USTRUCT()
struct FEconEventAbilityTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Description;
};
USTRUCT()
struct FEconEventAbilityAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
};

USTRUCT()
struct FEconEventAbility {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ID;

  UPROPERTY(EditAnywhere, SaveGame)
  FName EconEventId;  // * Linked econ event id.

  UPROPERTY(EditAnywhere, SaveGame)
  float Cost;  // * Cost in money.
  UPROPERTY(EditAnywhere, SaveGame)
  int32 DurationLeft;  // * In days.
  UPROPERTY(EditAnywhere, SaveGame)
  int32 Cooldown;  // * In days.

  UPROPERTY(EditAnywhere, SaveGame)
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere, SaveGame)
  FEconEventAbilityTextData TextData;
  UPROPERTY(EditAnywhere, SaveGame)
  FEconEventAbilityAssetData AssetData;
};
USTRUCT()
struct FEconEventAbilityRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FName EconEventId;  // * Linked econ event id.

  UPROPERTY(EditAnywhere)
  float Cost;  // * Cost in money.
  UPROPERTY(EditAnywhere)
  int32 Duration;  // * In days.
  UPROPERTY(EditAnywhere)
  int32 Cooldown;  // * In days.

  UPROPERTY(EditAnywhere)
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere)
  FEconEventAbilityTextData TextData;
  UPROPERTY(EditAnywhere)
  FEconEventAbilityAssetData AssetData;
};

// ! Currently 3 different ways to keep track of unlocked ids.
UENUM()
enum class EUpgradeEffectType : uint8 {
  ChangeBehaviorParam UMETA(DisplayName = "ChangeBehaviorParam"),  // * Changes a parameter value.
  ChangeData UMETA(DisplayName = "ChangeData"),                    // * Changes a parameter value in data manager.
  UnlockIDs UMETA(DisplayName = "UnlockIDs"),                      // * Function to unlock ids.
  FeatureUnlock UMETA(DisplayName = "FeatureUnlock"),              // * Unlocks a feature.
  UpgradeFunction UMETA(DisplayName = "UpgradeFunction"),          // * Custom named function.
};
ENUM_RANGE_BY_COUNT(EUpgradeEffectType, 4);
UENUM()
enum class EUpgradeEffectSystem : uint8 {
  Store UMETA(DisplayName = "Store"),                  // * Store upgrades.
  CustomerAI UMETA(DisplayName = "CustomerAI"),        // * Customer / behavior changes.
  Market UMETA(DisplayName = "Market"),                // * Market / behavior changes.
  MarketEconomy UMETA(DisplayName = "MarketEconomy"),  // * MarketEconomy changes.
  GlobalData UMETA(DisplayName = "GlobalData"),        // * GlobalData changes.
  Player UMETA(DisplayName = "Player"),                // * Player changes.
  Ability UMETA(DisplayName = "Ability"),              // * Ability changes.
};
ENUM_RANGE_BY_COUNT(EUpgradeEffectSystem, 7);

USTRUCT()
struct FUpgradeEffectTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Description;
};
USTRUCT()
struct FUpgradeEffectAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
};

USTRUCT()
struct FUpgradeEffect {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EUpgradeEffectType EffectType;
  UPROPERTY(EditAnywhere)
  EUpgradeEffectSystem EffectSystem;  // * Which system this effect influences.

  UPROPERTY(EditAnywhere)
  FName RelevantName;  // * Feature or function name.
  UPROPERTY(EditAnywhere)
  TArray<FName> RelevantIDs;  // * Item, npc, etc.
  UPROPERTY(EditAnywhere)
  TMap<FName, float> RelevantValues;  // * Parameter name to new value.

  UPROPERTY(EditAnywhere)
  FUpgradeEffectTextData TextData;
};
USTRUCT()
struct FUpgradeEffectRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EUpgradeEffectType EffectType;
  UPROPERTY(EditAnywhere)
  EUpgradeEffectSystem EffectSystem;  // * Which system this effect influences.

  UPROPERTY(EditAnywhere)
  TArray<FName> RelevantIDs;  // * Item, npc, etc.
  UPROPERTY(EditAnywhere)
  TMap<FName, float> RelevantValues;  // * Parameter name to new value.
  UPROPERTY(EditAnywhere)
  FName RelevantName;  // * Feature or function name.

  UPROPERTY(EditAnywhere)
  FUpgradeEffectTextData TextData;
};

USTRUCT()
struct FUpgradeTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Description;
};
USTRUCT()
struct FUpgradeAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
};

USTRUCT()
struct FUpgrade {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere)
  TArray<FName> UpgradeEffectIDs;

  UPROPERTY(EditAnywhere)
  int32 Cost;  // * Cost in upgrade points.
  UPROPERTY(EditAnywhere)
  int32 Level;  // * Arbitrary level for the upgrade. Higher than player levels wont be displayed.
  UPROPERTY(EditAnywhere)
  FName Requirements;  // * (RequirementsFilter structure) Won't be filtered out in the UI, just not available.
  UPROPERTY(EditAnywhere)
  FText RequirementsFilterDescription;  // * Description for the requirements filter to display in the UI.

  UPROPERTY(EditAnywhere)
  FUpgradeTextData TextData;
};
USTRUCT()
struct FUpgradeRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere)
  TArray<FName> UpgradeEffectIDs;

  UPROPERTY(EditAnywhere)
  int32 Cost;  // * Cost in upgrade points.
  UPROPERTY(EditAnywhere)
  int32 Level;  // * Arbitrary level for the upgrade. Higher than player levels wont be displayed.
  UPROPERTY(EditAnywhere)
  FName Requirements;  // * (RequirementsFilter structure) Won't be filtered out in the UI, just not available.
  UPROPERTY(EditAnywhere)
  FText RequirementsFilterDescription;  // * Description for the requirements filter to display in the UI.

  UPROPERTY(EditAnywhere)
  FUpgradeTextData TextData;
};

// * This allows a class to have a subset of functions.
USTRUCT()
struct FUpgradeable {
  GENERATED_BODY()

  // * Param name to new value.
  std::function<void(const TMap<FName, float>&)> ChangeBehaviorParam;
  // * Data name, array of ids to apply to (all if empty), and param name to new value.
  std::function<void(FName, const TArray<FName>&, const TMap<FName, float>&)> ChangeData;
  // * Data name, array of ids to unlock.
  std::function<void(FName, const TArray<FName>&)> UnlockIDs;
  // * Feature bool name to unlock.
  std::function<void(FName)> FeatureUnlock;
  // * Function name to call and parameters.
  std::function<void(FName, const TArray<FName>&, const TMap<FName, float>&)> UpgradeFunction;
};

// * Makes a class need to have all functions.
// struct Upgradeable {
//   template <class T>
//     requires(std::is_pointer_v<T>)
//   Upgradeable(T& object)
//       : ChangeBehaviorParam{[&object](const TMap<FName, float>& ParamValues) {
//           return object->ChangeBehaviorParam(ParamValues);
//         }},
//         UnlockIDs{[&object](FName DataName, const TArray<FName>& Ids) { return object->UnlockIDs(DataName, Ids); }} {}

//   std::function<void(const TMap<FName, float>&)> ChangeBehaviorParam;
//   std::function<void(FName, const TArray<FName>&)> UnlockIDs;
// };

// * No way to store dynamic classes.
// template <class T>
// concept UpgradeableO = requires(T o) {
//   { o->ChangeBehaviorParam(std::declval<const TMap<FName, float>&>()) } -> std::same_as<void>;
//   { o->UnlockIDs(std::declval<FName>(), std::declval<const TArray<FName>&>()) } -> std::same_as<void>;
// };

// template <class T>
//   requires(UpgradeableO<T>)
// void ChangeBehaviorParam(T& Upgradeable, const TMap<FName, float>& ParamValues) {
//   Upgradeable->ChangeBehaviorParam(ParamValues);
// }

// template <class T>
//   requires(UpgradeableO<T>)
// void UnlockIDs(T& Upgradeable, FName DataName, const TArray<FName>& Ids) {
//   Upgradeable->UnlockIDs(DataName, Ids);
// }