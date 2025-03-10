// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Framework/StorePGGameMode.h"
#include "Engine/World.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Player/PlayerZDCharacter.h"

AStorePGGameMode::AStorePGGameMode() {}

void AStorePGGameMode::BeginPlay() {
  Super::BeginPlay();

  APlayerZDCharacter* PlayerCharacter = Cast<APlayerZDCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
  check(PlayerCharacter && StorePhaseManagerClass && CustomerAIManagerClass && MarketClass && MarketEconomyClass &&
        StoreClass);

  // * Initialize the game world and all systems.
  AGlobalDataManager* GlobalDataManager = GetWorld()->SpawnActor<AGlobalDataManager>(GlobalDataManagerClass);
  AStorePhaseManager* StorePhaseManager = GetWorld()->SpawnActor<AStorePhaseManager>(StorePhaseManagerClass);
  ACustomerAIManager* CustomerAIManager = GetWorld()->SpawnActor<ACustomerAIManager>(CustomerAIManagerClass);
  AMarket* Market = GetWorld()->SpawnActor<AMarket>(MarketClass);
  AMarketEconomy* MarketEconomy = GetWorld()->SpawnActor<AMarketEconomy>(MarketEconomyClass);
  AStore* Store = GetWorld()->SpawnActor<AStore>(StoreClass);
  UDialogueSystem* DialogueSystem = NewObject<UDialogueSystem>(this);
  UNegotiationSystem* NegotiationSystem = NewObject<UNegotiationSystem>(this);

  PlayerCharacter->DialogueSystem = DialogueSystem;
  PlayerCharacter->NegotiationSystem = NegotiationSystem;
  PlayerCharacter->Store = Store;
  PlayerCharacter->StorePhaseManager = StorePhaseManager;
  PlayerCharacter->MarketEconomy = MarketEconomy;

  StorePhaseManager->Store = Store;
  StorePhaseManager->Market = Market;
  StorePhaseManager->CustomerAIManager = CustomerAIManager;

  CustomerAIManager->GlobalDataManager = GlobalDataManager;
  CustomerAIManager->MarketEconomy = MarketEconomy;
  CustomerAIManager->Store = Store;

  NegotiationSystem->MarketEconomy = MarketEconomy;
  NegotiationSystem->DialogueSystem = DialogueSystem;
  NegotiationSystem->Store = Store;

  StorePhaseManager->Start();
  Market->InitializeNPCStores();
  Store->InitStockDisplays();

  UE_LOG(LogTemp, Warning, TEXT("Game Initialized."));
}