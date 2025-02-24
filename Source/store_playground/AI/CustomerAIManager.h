// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "CustomerAIManager.generated.h"

extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> FriendlyDialoguesMap;
extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> NeutralDialoguesMap;
extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> HostileDialoguesMap;
std::unordered_map<ENegotiationDialogueType, std::vector<int32>> GetRandomNegDialogues();

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACustomerAIManager : public AInfo {
  GENERATED_BODY()

public:
  ACustomerAIManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Customer Blueprint")
  TSubclassOf<class ACustomer> CustomerClass;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle FriendlyDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle NeutralDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle HostileDialoguesTable;

  // TODO: Put in params struct.
  UPROPERTY(EditAnywhere, Category = "AI Params")
  float CustomerSpawnInterval;
  // * Upper limit overwriting dynamic limit.
  UPROPERTY(EditAnywhere, Category = "AI Params")
  int32 MaxCustomers;
  // * Chance for a customer to pick an item. Higher value means more customers wanting to buy at any one time.
  UPROPERTY(EditAnywhere, Category = "AI Params")
  float PickItemChance;
  UPROPERTY(EditAnywhere, Category = "AI Params")
  float PickItemFrequency;
  UPROPERTY(EditAnywhere, Category = "AI Params")
  float LastPickItemCheckTime;

  UPROPERTY(EditAnywhere, Category = "AI Timer")
  FTimerHandle PickItemTimer;

  UPROPERTY(EditAnywhere, Category = "Player Stock")
  const class UInventoryComponent* PlayerStock;

  UPROPERTY(EditAnywhere, Category = "Customers Array")
  TArray<class ACustomer*> AllCustomers;
  // ? Have separate arrays for each state?
  // UPROPERTY(EditAnywhere, Category = "Customers Array")
  // TArray<class UCustomerAIComponent*> BrowsingCustomers;
  // UPROPERTY(EditAnywhere, Category = "Customers Array")
  // TArray<class UCustomerAIComponent*> ItemPickedCustomers;
  void PerformCustomerAILoop();

  void CustomerPickItem(class ACustomer* Customer);

  // ? Put in a separate class?
  // * Customer spawning logic.
  void SpawnCustomers();
  // * Initializes the dialogue data from data tables.
  void InitializeDialogueData();
};