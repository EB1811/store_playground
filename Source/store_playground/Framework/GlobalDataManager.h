// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <any>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "GlobalDataManager.generated.h"

// * Dynamic data that could change during the game, e.g., from upgrades.

UENUM()
enum class EReqFilterOperand : uint8 {
  Time UMETA(DisplayName = "Time"),
  Money UMETA(DisplayName = "Money"),
  Inventory UMETA(DisplayName = "Inventory"),                      // * Using item ids.
  QuestsCompleted UMETA(DisplayName = "QuestsCompleted"),          // * Using quest ids.
  MadeDialogueChoices UMETA(DisplayName = "MadeDialogueChoices"),  // * Using dialogue chain ids.
  RecentEconEvents UMETA(DisplayName = "RecentEconEvents"),        // * Using econ event ids.
};
ENUM_RANGE_BY_COUNT(EReqFilterOperand, 5);

bool EvaluatePlayerTagsRequirements(const FGameplayTagContainer& RequiredTags, const class UTagsComponent* PlayerTagsC);
bool EvaluateRequirementsFilter(const FName& RequirementsFilter, const TMap<EReqFilterOperand, std::any>& GameDataMap);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AGlobalDataManager : public AInfo {
  GENERATED_BODY()

public:
  AGlobalDataManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class APlayerZDCharacter* PlayerCharacter;
  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class AQuestManager* QuestManager;
  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class AMarket* Market;
  // todo-low: Allow getting only specific operands.
  const TMap<EReqFilterOperand, std::any> GetGameDataMap() const;

  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> NpcStoreTypesDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> EconEventsDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> EconEventAbilitiesTable;

  UPROPERTY(EditAnywhere, Category = "NpcStore")
  TArray<struct FNpcStoreType> NpcStoreTypesArray;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FEconEvent> EconEventsArray;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TArray<struct FEconEventAbility> EconEventAbilitiesArray;

  auto GetEligibleEconEvents(const TArray<FName>& OccurredEconEvents) const -> TArray<struct FEconEvent>;
  auto GetEconEventsByIds(const TArray<FName>& EventIDs) const -> TArray<struct FEconEvent>;

  auto GetEconEventAbilitiesByIds(const TArray<FName>& AbilityIDs) const -> TArray<struct FEconEventAbility>;

  UPROPERTY(EditAnywhere, Category = "GlobalDataManager")
  FUpgradeable Upgradeable;
  void ChangeData(FName DataName, const TArray<FName>& FilterIds, const TMap<FName, float>& ParamValues);

  void InitializeNpcStoreData();
  void InitializeMarketData();
  void InitializeUpgradesData();
};