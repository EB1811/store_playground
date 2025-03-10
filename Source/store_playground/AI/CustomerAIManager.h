// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Store/Store.h"
#include "CustomerAIManager.generated.h"

USTRUCT()
struct FManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bSpawnCustomers;
  UPROPERTY(EditAnywhere)
  float CustomerSpawnInterval;
  UPROPERTY(EditAnywhere)
  int32 MaxCustomers;

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
  UPROPERTY(EditAnywhere)
  int32 UNpcMaxSpawnPerDay;

  UPROPERTY(EditAnywhere)
  float PickItemChance;
  UPROPERTY(EditAnywhere)
  float PickItemFrequency;
  UPROPERTY(EditAnywhere)
  int32 MaxCustomersPickingAtOnce;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACustomerAIManager : public AInfo {
  GENERATED_BODY()

public:
  ACustomerAIManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Customer Blueprint")
  TSubclassOf<class ACustomer> CustomerClass;

  const class AGlobalDataManager* GlobalDataManager;
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere, Category = "Manager Params")
  struct FManagerParams ManagerParams;

  UPROPERTY(EditAnywhere, Category = "Spawn Customers")
  float LastSpawnTime;

  UPROPERTY(EditAnywhere, Category = "Pick Item")
  float LastPickItemCheckTime;
  UPROPERTY(EditAnywhere, Category = "Pick Item")
  FTimerHandle PickItemTimer;
  UPROPERTY(EditAnywhere, Category = "Pick Item")
  TArray<FGuid> CustomersPickingIds;

  UPROPERTY(EditAnywhere, Category = "Player Stock")
  const class AStore* Store;

  // ? Have separate arrays for each state?
  UPROPERTY(EditAnywhere, Category = "Customers Array")
  TArray<class ACustomer*> AllCustomers;

  void StartCustomerAI();
  void EndCustomerAI();

  void SpawnCustomers();
  void PerformCustomerAILoop();

  void CustomerPickItem(class UCustomerAIComponent* CustomerAI, class UInteractionComponent* Interaction);
  void UniqueNpcPickItem(class UCustomerAIComponent* CustomerAI, class UInteractionComponent* Interaction);
  void CustomerStockCheck(class UCustomerAIComponent* CustomerAI, class UInteractionComponent* Interaction);
};