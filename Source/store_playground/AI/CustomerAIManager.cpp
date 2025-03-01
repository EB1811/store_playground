
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
  check(StoreStock);

  LastPickItemCheckTime = GetWorld()->GetTimeSeconds();

  // ? Call function somewhere to remove the picked item on successful negotiation?
  CustomersPickingIds = CustomersPickingIds.FilterByPredicate([this](FGuid ItemID) {
    return StoreStock->ItemsArray.ContainsByPredicate(
        [ItemID](UItemBase* Item) { return Item->UniqueItemID == ItemID; });
  });

  std::vector<ACustomer*> CustomersToRemove;

  for (ACustomer* Customer : AllCustomers) {
    switch (Customer->CustomerAIComponent->CustomerState) {
      case (ECustomerState::Browsing): {
        if (CustomersPickingIds.Num() >= ManagerParams.MaxCustomersPickingAtOnce) break;

        if (FMath::FRand() * 100 < ManagerParams.PickItemChance) {
          if (Customer->CustomerAIComponent->CustomerType == ECustomerType::Unique)
            UniqueNpcPickItem(Customer->CustomerAIComponent, Customer->InteractionComponent);
          else
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
        CustomersToRemove.push_back(Customer);
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

void ACustomerAIManager::CustomerPickItem(class UCustomerAIComponent* CustomerAI,
                                          class UInteractionComponent* Interaction) {
  if (StoreStock->ItemsArray.Num() <= 0 || CustomersPickingIds.Num() >= StoreStock->ItemsArray.Num()) return;

  // TODO: Picked items will depend on customer type.
  TArray<UItemBase*> NonPickedItems = StoreStock->ItemsArray.FilterByPredicate(
      [this](UItemBase* Item) { return !CustomersPickingIds.Contains(Item->UniqueItemID); });

  int32 RandomIndex = FMath::RandRange(0, NonPickedItems.Num() - 1);
  const UItemBase* Item = NonPickedItems[RandomIndex];
  // ? Use item id instead of item pointer?
  CustomerAI->NegotiationAI->WantedItem = Item;
  CustomersPickingIds.Add(Item->UniqueItemID);

  // TODO: Move state management to customer ai component.
  CustomerAI->CustomerState = ECustomerState::ItemWanted;
  Interaction->InteractionType = EInteractionType::WaitingCustomer;

  CustomerAI->NegotiationAI->AcceptancePercentage = FMath::FRandRange(1.05f, 1.15f);

  auto RandomNegDialogueMap = GlobalDataManager->GetRandomNegDialogueMap();
  CustomerAI->NegotiationAI->RequestDialogueArray = RandomNegDialogueMap[ENegotiationDialogueType::Request].Dialogues;
  CustomerAI->NegotiationAI->ConsiderTooHighArray =
      RandomNegDialogueMap[ENegotiationDialogueType::ConsiderTooHigh].Dialogues;
  CustomerAI->NegotiationAI->ConsiderCloseArray =
      RandomNegDialogueMap[ENegotiationDialogueType::ConsiderClose].Dialogues;
  CustomerAI->NegotiationAI->AcceptArray = RandomNegDialogueMap[ENegotiationDialogueType::Accept].Dialogues;
  CustomerAI->NegotiationAI->RejectArray = RandomNegDialogueMap[ENegotiationDialogueType::Reject].Dialogues;
}

void ACustomerAIManager::UniqueNpcPickItem(class UCustomerAIComponent* CustomerAI,
                                           class UInteractionComponent* Interaction) {
  if (StoreStock->ItemsArray.Num() <= 0 || CustomersPickingIds.Num() >= StoreStock->ItemsArray.Num()) return;

  TArray<UItemBase*> NonPickedItems = StoreStock->ItemsArray.FilterByPredicate(
      [this](UItemBase* Item) { return !CustomersPickingIds.Contains(Item->UniqueItemID); });
  TArray<UItemBase*> WantedItems = NonPickedItems.FilterByPredicate(
      [this, CustomerAI](UItemBase* Item) { return CustomerAI->WantedBaseItemIDs.Contains(Item->ItemID); });
  if (WantedItems.Num() <= 0) return;

  int32 RandomIndex = FMath::RandRange(0, WantedItems.Num() - 1);
  const UItemBase* Item = WantedItems[RandomIndex];
  CustomerAI->NegotiationAI->WantedItem = Item;
  CustomersPickingIds.Add(Item->UniqueItemID);

  CustomerAI->CustomerState = ECustomerState::ItemWanted;
  Interaction->InteractionType = EInteractionType::WaitingUniqueCustomer;

  auto RandomNegDialogueMap = GlobalDataManager->GetRandomNegDialogueMap();
  CustomerAI->NegotiationAI->RequestDialogueArray = RandomNegDialogueMap[ENegotiationDialogueType::Request].Dialogues;
  CustomerAI->NegotiationAI->ConsiderTooHighArray =
      RandomNegDialogueMap[ENegotiationDialogueType::ConsiderTooHigh].Dialogues;
  CustomerAI->NegotiationAI->ConsiderCloseArray =
      RandomNegDialogueMap[ENegotiationDialogueType::ConsiderClose].Dialogues;
  CustomerAI->NegotiationAI->AcceptArray = RandomNegDialogueMap[ENegotiationDialogueType::Accept].Dialogues;
  CustomerAI->NegotiationAI->RejectArray = RandomNegDialogueMap[ENegotiationDialogueType::Reject].Dialogues;
}