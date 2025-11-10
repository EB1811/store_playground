// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameplayTagContainer.h"
#include "store_playground/StatisticsGen/StatisticsStructs.h"
#include "SteamManager.generated.h"

// AInfo rather than subsystem to access global data managers.
// Mix of generic, scripted achievements, and custom (if statement) achievements.

UENUM()
enum class ESteamAchievementType : uint8 {
  GameState UMETA(DisplayName = "GameState"),  // * E.g., Money > X, NetWorth > X, etc.
  Triggered UMETA(DisplayName = "Triggered"),  // * E.g., Complete the intro, etc.
  Tracked UMETA(DisplayName = "Tracked"),  // * E.g., tracked Behaviour, talk to npc x times, sell item x times, etc.
  Custom UMETA(DisplayName = "Custom"),    // * Custom handling.
};
USTRUCT()
struct FSteamAchievement {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName SteamID;
  UPROPERTY(EditAnywhere)
  FGameplayTag IdTag;  // * For semantic, constant ids.

  UPROPERTY(EditAnywhere)
  ESteamAchievementType Type;

  UPROPERTY(EditAnywhere)
  FName Requirements;  // * (RequirementsFilter structure). Required for gamestate, optional for others.
  UPROPERTY(EditAnywhere)
  FGameplayTag AchBehaviourTag;  // * For Triggered and Tracked types.
  UPROPERTY(EditAnywhere)
  int32 TargetCount;  // * For Tracked type.

  UPROPERTY(EditAnywhere, SaveGame)
  bool bAchieved;
};

USTRUCT()
struct FSteamManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bEnableSteamIntegration;  // * Whether to send api calls to steam.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ASteamManager : public AInfo {
  GENERATED_BODY()

public:
  ASteamManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class AGlobalDataManager* GlobalDataManager;

  UPROPERTY(EditAnywhere)
  FSteamManagerParams SteamManagerParams;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FSteamAchievement> SteamAchievements;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FGameplayTag, int32> BehaviourTagCounts;

  void QueryAchievements();
  void AwardAchievements(TArray<FSteamAchievement>& Achievements);

  void ConsiderAchievements();
  void AchBehaviour(FGameplayTag AchBehaviourTag);
  // * Custom achievements.
  void AchItemDeal(const FItemDeal ItemDeal);
};