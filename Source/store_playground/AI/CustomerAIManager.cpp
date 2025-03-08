
#include "CustomerAIManager.h"
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

ACustomerAIManager::ACustomerAIManager() {
  PrimaryActorTick.bCanEverTick = true;

  ManagerParams.bSpawnCustomers = true;
  ManagerParams.CustomerSpawnInterval = 5.0f;
  ManagerParams.MaxCustomers = 1;

  ManagerParams.UniqueNpcBaseSpawnChance = 10.0f;
  ManagerParams.UNpcMaxSpawnPerDay = 1;

  ManagerParams.PickItemChance = 50.0f;
  ManagerParams.PickItemFrequency = 5.0f;
  ManagerParams.MaxCustomersPickingAtOnce = 2;

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
  check(CustomerClass);
  if (AllCustomers.Num() >= ManagerParams.MaxCustomers) return;

  LastSpawnTime = GetWorld()->GetTimeSeconds();

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  // TODO: Spawn conditions.
  if (ManagerParams.UniqueNpcBaseSpawnChance > FMath::FRand() * 100) {
    FUniqueNpcData UniqueNpcData = GlobalDataManager->GetRandomUniqueNpcData();
    UE_LOG(LogTemp, Warning, TEXT("Spawning unique npc: %s."), *UniqueNpcData.NpcID.ToString());
    ACustomer* UniqueCustomer = GetWorld()->SpawnActor<ACustomer>(
        CustomerClass, GetActorLocation() + (GetActorForwardVector() + FMath::FRandRange(20.0f, 500.0f)),
        GetActorRotation(), SpawnParams);

    UniqueCustomer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    // TODO: Dialogue tracking.
    FName RandomDialogueChainID =
        UniqueNpcData.DialogueChainIDs[FMath::RandRange(0, UniqueNpcData.DialogueChainIDs.Num() - 1)];
    UniqueCustomer->DialogueComponent->DialogueArray =
        GlobalDataManager->UniqueNpcDialoguesMap[RandomDialogueChainID].Dialogues;

    UniqueCustomer->CustomerAIComponent->CustomerType = ECustomerType::Unique;
    UniqueCustomer->CustomerAIComponent->Attitude = ECustomerAttitude::Neutral;
    UniqueCustomer->CustomerAIComponent->WantedBaseItemIDs = UniqueNpcData.WantedBaseItemIDs;

    UniqueCustomer->CustomerAIComponent->NegotiationAI->AcceptancePercentage =
        FMath::FRandRange(UniqueNpcData.AcceptancePercentageRange[0], UniqueNpcData.AcceptancePercentageRange[1]) /
        100.0f;

    // ? Maybe have different arrays for different customer types?
    AllCustomers.Add(UniqueCustomer);
  }

  if (AllCustomers.Num() >= ManagerParams.MaxCustomers) return;

  int32 CustomersToSpawn = FMath::RandRange(1, ManagerParams.MaxCustomers - AllCustomers.Num());
  UE_LOG(LogTemp, Warning, TEXT("Spawning %d customers."), CustomersToSpawn);
  for (int32 i = 0; i < CustomersToSpawn; i++) {
    ACustomer* Customer = GetWorld()->SpawnActor<ACustomer>(
        CustomerClass, GetActorLocation() + (GetActorForwardVector() + FMath::FRandRange(20.0f, 500.0f)),
        GetActorRotation(), SpawnParams);

    Customer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    // TODO: Add leaving dialogue.
    Customer->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomCustomerDialogue();

    Customer->CustomerAIComponent->CustomerType = ECustomerType::Peasant;
    Customer->CustomerAIComponent->Attitude = ECustomerAttitude::Friendly;

    // ? Maybe have two arrays for customers ai and customer interaction components?
    AllCustomers.Add(Customer);
  }
}

// TODO: Monitor performance.
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

        if (FMath::FRand() * 100 < ManagerParams.PickItemChance) {
          if (Customer->CustomerAIComponent->CustomerType == ECustomerType::Unique)
            UniqueNpcPickItem(Customer->CustomerAIComponent, Customer->InteractionComponent);
          else
            // CustomerStockCheck(Customer->CustomerAIComponent, Customer->InteractionComponent);
            CustomerPickItem(Customer->CustomerAIComponent, Customer->InteractionComponent);
        }
        break;
      }
      case (ECustomerState::ItemWanted): {
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

    Customer->Destroy();
  }
}

// ? Use item ids to pick items?
void ACustomerAIManager::CustomerPickItem(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  if (Store->StoreStockItems.Num() <= 0 || CustomersPickingIds.Num() >= Store->StoreStockItems.Num()) return;

  // TODO: Picked items will depend on customer type.
  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !CustomersPickingIds.Contains(StockItem.Item->UniqueItemID); });

  int32 RandomIndex = FMath::RandRange(0, NonPickedItems.Num() - 1);
  const FStockItem& StockItem = NonPickedItems[RandomIndex];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAI->StockDisplayInventory = StockItem.BelongingStockInventoryC;

  CustomersPickingIds.Add(StockItem.Item->UniqueItemID);

  // TODO: Move state management to customer ai component.
  CustomerAI->CustomerState = ECustomerState::ItemWanted;
  Interaction->InteractionType = EInteractionType::WaitingCustomer;

  CustomerAI->NegotiationAI->AcceptancePercentage = FMath::FRandRange(1.05f, 1.15f);

  CustomerAI->NegotiationAI->DialoguesMap = GlobalDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);
}

void ACustomerAIManager::UniqueNpcPickItem(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  if (Store->StoreStockItems.Num() <= 0 || CustomersPickingIds.Num() >= Store->StoreStockItems.Num()) return;

  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !CustomersPickingIds.Contains(StockItem.Item->UniqueItemID); });
  TArray<FStockItem> RelevantItems = NonPickedItems.FilterByPredicate([this, CustomerAI](const FStockItem& StockItem) {
    return CustomerAI->WantedBaseItemIDs.Contains(StockItem.Item->ItemID);
  });
  if (RelevantItems.Num() <= 0) return;

  int32 RandomIndex = FMath::RandRange(0, RelevantItems.Num() - 1);
  const FStockItem& StockItem = RelevantItems[RandomIndex];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAI->StockDisplayInventory = StockItem.BelongingStockInventoryC;

  CustomersPickingIds.Add(StockItem.Item->UniqueItemID);

  CustomerAI->CustomerState = ECustomerState::ItemWanted;
  Interaction->InteractionType = EInteractionType::WaitingUniqueCustomer;

  CustomerAI->NegotiationAI->DialoguesMap = GlobalDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);
}

void ACustomerAIManager::CustomerStockCheck(UCustomerAIComponent* CustomerAI, UInteractionComponent* Interaction) {
  if (Store->StoreStockItems.Num() <= 0 || CustomersPickingIds.Num() >= Store->StoreStockItems.Num()) return;

  // Temp
  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !CustomersPickingIds.Contains(StockItem.Item->UniqueItemID); });

  int32 RandomIndex = FMath::RandRange(0, NonPickedItems.Num() - 1);
  const FStockItem& StockItem = NonPickedItems[RandomIndex];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::StockCheck;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;

  CustomersPickingIds.Add(StockItem.Item->UniqueItemID);

  CustomerAI->CustomerState = ECustomerState::ItemWanted;
  Interaction->InteractionType = EInteractionType::WaitingCustomer;

  CustomerAI->NegotiationAI->DialoguesMap = GlobalDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);
}