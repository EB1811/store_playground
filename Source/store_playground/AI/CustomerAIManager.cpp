
#include "CustomerAIManager.h"
#include <functional>
#include <vector>
#include "Containers/Array.h"
#include "CustomerAIComponent.h"
#include "NegotiationAI.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "store_playground/WorldObject/Customer.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"

template <typename T>
T GetWeightedRandomItem(const TArray<T>& Items, std::function<float(const T&)> WeightFunc) {
  float TotalWeight = 0.0f;
  for (const T& Item : Items) TotalWeight += WeightFunc(Item);

  float RandomItem = FMath::FRandRange(0.0f, TotalWeight);
  for (const T& Item : Items) {
    if (RandomItem < WeightFunc(Item)) return Item;
    RandomItem -= WeightFunc(Item);
  }

  return Items[0];
}

ACustomerAIManager::ACustomerAIManager() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;

  ManagerParams.bSpawnCustomers = true;
  ManagerParams.CustomerSpawnInterval = 5.0f;
  ManagerParams.MaxCustomers = 1;

  ManagerParams.UniqueNpcBaseSpawnChance = 10.0f;
  ManagerParams.UNpcMaxSpawnPerDay = 1;

  ManagerParams.PickItemFrequency = 5.0f;
  ManagerParams.MaxCustomersPickingAtOnce = 2;

  ManagerParams.PerformActionChance = 50.0f;
  ManagerParams.ActionWeights = {
      {ECustomerAction::PickItem, 50.0f},
      {ECustomerAction::StockCheck, 25.0f},
      {ECustomerAction::SellItem, 15.0f},
      {ECustomerAction::Leave, 10.0f},
  };

  LastSpawnTime = 0.0f;
  LastPickItemCheckTime = 0.0f;
}

void ACustomerAIManager::BeginPlay() { Super::BeginPlay(); }

void ACustomerAIManager::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (ManagerParams.bSpawnCustomers && GetWorld()->TimeSince(LastSpawnTime) > ManagerParams.CustomerSpawnInterval)
    SpawnCustomers();
  if (GetWorld()->TimeSince(LastPickItemCheckTime) > ManagerParams.PickItemFrequency) PerformCustomerAILoop();
}

void ACustomerAIManager::StartCustomerAI() {
  ManagerParams.bSpawnCustomers = true;
  SetActorTickEnabled(true);
}

void ACustomerAIManager::EndCustomerAI() {
  ManagerParams.bSpawnCustomers = false;
  SetActorTickEnabled(false);

  CustomersPickingIds.Empty();

  TArray<ACustomer*> CustomersToRemove;
  for (ACustomer* Customer : AllCustomers) CustomersToRemove.Add(Customer);
  AllCustomers.Empty();
  for (ACustomer* Customer : CustomersToRemove) Customer->Destroy();
}

void ACustomerAIManager::SpawnCustomers() {
  check(CustomerClass && GlobalDataManager && MarketEconomy && Store);
  if (AllCustomers.Num() >= ManagerParams.MaxCustomers) return;

  LastSpawnTime = GetWorld()->GetTimeSeconds();

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  if (ManagerParams.UniqueNpcBaseSpawnChance >= FMath::FRand() * 100) {
    // Temp: Need info about player.
    FFilterGameData GameData = {Store->Money, QuestsCompleted};
    TArray<struct FUniqueNpcData> EligibleNpcs = GlobalDataManager->GetEligibleNpcs(GameData);
    if (EligibleNpcs.Num() <= 0) return;

    FUniqueNpcData UniqueNpcData =
        GetWeightedRandomItem<FUniqueNpcData>(EligibleNpcs, [](const auto& Npc) { return Npc.SpawnChanceWeight; });

    UE_LOG(LogTemp, Warning, TEXT("Spawning unique npc: %s."), *UniqueNpcData.NpcID.ToString());
    ACustomer* UniqueCustomer = GetWorld()->SpawnActor<ACustomer>(
        CustomerClass, GetActorLocation() + (GetActorForwardVector() + FMath::FRandRange(20.0f, 500.0f)),
        GetActorRotation(), SpawnParams);

    TArray<struct FQuestChainData> EligibleQuestChains =
        GlobalDataManager->GetEligibleQuestChains(UniqueNpcData.QuestIDs, GameData, QuestsInProgressMap);
    if (EligibleQuestChains.Num() > 0) {
      // Most likely quest chain.
      FQuestChainData RandomQuestChainData = GetWeightedRandomItem<FQuestChainData>(
          EligibleQuestChains, [](const auto& Chain) { return Chain.StartChance; });
      if (FMath::FRand() * 100 < RandomQuestChainData.StartChance) {
        // Temp: Just dialogue for now.
        UniqueCustomer->DialogueComponent->DialogueArray =
            GlobalDataManager->GetRandomNpcDialogue({RandomQuestChainData.DialogueChainID});

        QuestsInProgressMap.Add(RandomQuestChainData.QuestID, RandomQuestChainData.QuestChainID);
      }
    }

    if (UniqueCustomer->DialogueComponent->DialogueArray.Num() <= 0)
      UniqueCustomer->DialogueComponent->DialogueArray =
          GlobalDataManager->GetRandomNpcDialogue(UniqueNpcData.DialogueChainIDs);

    UniqueCustomer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    UniqueCustomer->CustomerAIComponent->CustomerType = ECustomerType::Unique;
    UniqueCustomer->CustomerAIComponent->Attitude = ECustomerAttitude::Neutral;
    UniqueCustomer->CustomerAIComponent->WantedBaseItemIDs = UniqueNpcData.WantedBaseItemIDs;

    UniqueCustomer->CustomerAIComponent->NegotiationAI->AcceptancePercentage =
        FMath::FRandRange(UniqueNpcData.NegotiationData.AcceptancePercentageRange[0],
                          UniqueNpcData.NegotiationData.AcceptancePercentageRange[1]) /
        100.0f;

    AllCustomers.Add(UniqueCustomer);
  }

  int32 CustomersToSpawn = FMath::RandRange(1, ManagerParams.MaxCustomers - AllCustomers.Num());
  UE_LOG(LogTemp, Warning, TEXT("Spawning %d customers."), CustomersToSpawn);
  for (int32 i = 0; i < CustomersToSpawn; i++) {
    ACustomer* Customer = GetWorld()->SpawnActor<ACustomer>(
        CustomerClass, GetActorLocation() + (GetActorForwardVector() + FMath::FRandRange(20.0f, 500.0f)),
        GetActorRotation(), SpawnParams);

    // TODO: Spawn probabilities with weights.
    const FGenericCustomerData RandomCustomerData =
        GlobalDataManager
            ->GenericCustomersArray[FMath::RandRange(0, GlobalDataManager->GenericCustomersArray.Num() - 1)];
    const FCustomerPop* CustomerPopData = MarketEconomy->AllCustomerPops.FindByPredicate(
        [RandomCustomerData](const FCustomerPop& Pop) { return Pop.PopID == RandomCustomerData.LinkedPopID; });
    const FPopMoneySpendData* PopMoneySpendData = MarketEconomy->PopMoneySpendDataArray.FindByPredicate(
        [RandomCustomerData](const FPopMoneySpendData& Pop) { return Pop.PopID == RandomCustomerData.LinkedPopID; });
    check(CustomerPopData && PopMoneySpendData);

    Customer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    Customer->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomCustomerDialogue();

    Customer->CustomerAIComponent->CustomerType = ECustomerType::Generic;
    Customer->CustomerAIComponent->ItemEconTypes = CustomerPopData->ItemEconTypes;
    Customer->CustomerAIComponent->MoneyToSpend = PopMoneySpendData->Money / PopMoneySpendData->Population;
    Customer->CustomerAIComponent->Attitude = RandomCustomerData.InitAttitude;

    // ? Maybe have two arrays for customers ai and customer interaction components?
    AllCustomers.Add(Customer);
  }
}

void ACustomerAIManager::PerformCustomerAILoop() {
  check(Store);

  LastPickItemCheckTime = GetWorld()->GetTimeSeconds();

  // ? Call function somewhere to remove the picked item on successful negotiation?
  CustomersPickingIds = CustomersPickingIds.FilterByPredicate([this](FGuid ItemID) {
    return Store->StoreStockItems.ContainsByPredicate(
        [ItemID](const FStockItem& StockItem) { return StockItem.Item->UniqueItemID == ItemID; });
  });

  TArray<ACustomer*> CustomersToRemove;

  for (ACustomer* Customer : AllCustomers) {
    switch (Customer->CustomerAIComponent->CustomerState) {
      case (ECustomerState::Browsing): {
        if (CustomersPickingIds.Num() >= ManagerParams.MaxCustomersPickingAtOnce) break;

        if (FMath::FRand() * 100 < ManagerParams.PerformActionChance)
          CustomerPerformAction(Customer->CustomerAIComponent, Customer->InteractionComponent);
        break;
      }
      case (ECustomerState::Requesting): {
        // ? Maybe have a timer for how long the customer waits for the item.
        break;
      }
      case (ECustomerState::Negotiating): {
        break;
      }
      case (ECustomerState::Leaving): {
        CustomersToRemove.Add(Customer);
        break;
      }
      default: break;
    }
  }

  for (ACustomer* Customer : CustomersToRemove) {
    AllCustomers.RemoveSingle(Customer);
    // ? Store struct of picked ids and associated customer?
    if (Customer->CustomerAIComponent->NegotiationAI) {
      if (Customer->CustomerAIComponent->NegotiationAI->RelevantItem)
        CustomersPickingIds.RemoveSingle(Customer->CustomerAIComponent->NegotiationAI->RelevantItem->UniqueItemID);

      Customer->CustomerAIComponent->NegotiationAI->RelevantItem = nullptr;
      Customer->CustomerAIComponent->NegotiationAI = nullptr;
    }
    Customer->Destroy();
  }
}

void ACustomerAIManager::CustomerPerformAction(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  check(CustomerAI && Interaction);

  // TODO: Specify unique npc action to take.
  if (CustomerAI->CustomerType == ECustomerType::Unique) return UniqueNpcPickItem(CustomerAI, Interaction);

  TMap<ECustomerAction, float> ActionWeights = ManagerParams.ActionWeights;
  if (Store->StoreStockItems.Num() < 5.0f)
    ActionWeights[ECustomerAction::PickItem] -= (5.0f - Store->StoreStockItems.Num()) * 5.0f;
  ECustomerAction RandomAction =
      GetWeightedRandomItem<TTuple<ECustomerAction, float>>(ActionWeights.Array(), [](const auto& Action) {
        return Action.Value;
      }).Key;

  switch (RandomAction) {
    case (ECustomerAction::PickItem): {
      CustomerPickItem(CustomerAI, Interaction);
      break;
    }
    case (ECustomerAction::StockCheck): {
      CustomerStockCheck(CustomerAI, Interaction);
      break;
    }
    case (ECustomerAction::SellItem): {
      CustomerSellItem(CustomerAI, Interaction);
      break;
    }
    case (ECustomerAction::Leave): {
      CustomerAI->CustomerState = ECustomerState::Leaving;
      break;
    }
    default: break;
  }
}

// ? Use item ids to pick items?
void ACustomerAIManager::CustomerPickItem(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  if (Store->StoreStockItems.Num() <= 0 || CustomersPickingIds.Num() >= Store->StoreStockItems.Num()) return;

  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !CustomersPickingIds.Contains(StockItem.Item->UniqueItemID); });
  TArray<FStockItem> RelevantItems = NonPickedItems.FilterByPredicate([this, CustomerAI](const FStockItem& StockItem) {
    return CustomerAI->ItemEconTypes.Contains(StockItem.Item->ItemEconType) &&
           CustomerAI->MoneyToSpend >= StockItem.Item->PriceData.BoughtAt;
  });

  const FStockItem& StockItem = RelevantItems[FMath::RandRange(0, RelevantItems.Num() - 1)];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAI->StockDisplayInventory = StockItem.BelongingStockInventoryC;

  CustomersPickingIds.Add(StockItem.Item->UniqueItemID);

  MakeCustomerNegotiable(CustomerAI, Interaction);
}

void ACustomerAIManager::CustomerStockCheck(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  // Note: Agnostic about if player has item type or not.

  TArray<FWantedItemType> ValidItemTypes =
      GlobalDataManager->WantedItemTypesArray.FilterByPredicate([CustomerAI](const FWantedItemType& ItemType) {
        return CustomerAI->ItemEconTypes.Contains(ItemType.ItemEconType);
      });
  if (ValidItemTypes.Num() <= 0) return;

  const FWantedItemType& WantedItemType = ValidItemTypes[FMath::RandRange(0, ValidItemTypes.Num() - 1)];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::StockCheck;
  CustomerAI->NegotiationAI->WantedItemType = WantedItemType;

  MakeCustomerNegotiable(CustomerAI, Interaction);
}

void ACustomerAIManager::CustomerSellItem(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  // Temp: Random item.
  UItemBase* Item = Market->GetNewRandomItems(1)[0];
  check(Item);

  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::SellItem;
  CustomerAI->NegotiationAI->RelevantItem = Item;

  MakeCustomerNegotiable(CustomerAI, Interaction);
}

void ACustomerAIManager::MakeCustomerNegotiable(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  CustomerAI->CustomerState = ECustomerState::Requesting;
  Interaction->InteractionType = EInteractionType::WaitingCustomer;

  float AcceptanceMin = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 0.05f
                        : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 0.00f
                                                                             : 0.05f;
  float AcceptanceMax = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 0.25f
                        : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 0.10f
                                                                             : 0.15f;
  CustomerAI->NegotiationAI->AcceptancePercentage = FMath::FRandRange(AcceptanceMin, AcceptanceMax);
  CustomerAI->NegotiationAI->DialoguesMap = GlobalDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);
}

void ACustomerAIManager::UniqueNpcPickItem(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  if (Store->StoreStockItems.Num() <= 0 || CustomersPickingIds.Num() >= Store->StoreStockItems.Num()) return;

  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !CustomersPickingIds.Contains(StockItem.Item->UniqueItemID); });
  TArray<FStockItem> RelevantItems = NonPickedItems.FilterByPredicate([CustomerAI](const FStockItem& StockItem) {
    if (CustomerAI->WantedBaseItemIDs.Num() > 0)
      return CustomerAI->WantedBaseItemIDs.Contains(StockItem.Item->ItemID);
    else
      return CustomerAI->ItemEconTypes.Contains(StockItem.Item->ItemEconType) &&
             CustomerAI->MoneyToSpend >= StockItem.Item->PriceData.BoughtAt;
  });
  if (RelevantItems.Num() <= 0) return;

  int32 RandomIndex = FMath::RandRange(0, RelevantItems.Num() - 1);
  const FStockItem& StockItem = RelevantItems[RandomIndex];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAI->StockDisplayInventory = StockItem.BelongingStockInventoryC;

  CustomersPickingIds.Add(StockItem.Item->UniqueItemID);

  CustomerAI->CustomerState = ECustomerState::Requesting;
  Interaction->InteractionType = EInteractionType::WaitingUniqueCustomer;

  CustomerAI->NegotiationAI->DialoguesMap = GlobalDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);
}