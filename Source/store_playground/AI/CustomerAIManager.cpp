#include "CustomerAIManager.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "CustomerAIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/AssertionMacros.h"
#include "NegotiationAI.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "store_playground/WorldObject/Customer.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/NPC/NpcDataStructs.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NegotiationAI.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/WidgetComponent.h"
#include "PaperFlipbookComponent.h"

ACustomerAIManager::ACustomerAIManager() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;

  ManagerParams.bSpawnCustomers = false;
  ManagerParams.CustomerSpawnInterval = 5.0f;
  ManagerParams.UniqueNpcBaseSpawnChance = 10.0f;
  ManagerParams.RecentNpcSpawnedKeepTime = 2.0f;
  ManagerParams.UNpcMaxSpawnPerDay = 1;
  ManagerParams.PickItemFrequency = 5.0f;
  ManagerParams.MaxCustomersPickingAtOnce = 2;

  BehaviorParams.MaxCustomers = 1;
  BehaviorParams.CustomerSpawnChance = 50.0f;
  BehaviorParams.PerformActionChance = 50.0f;
  BehaviorParams.ActionWeights = {
      {ECustomerAction::PickItem, 50.0f},
      {ECustomerAction::StockCheck, 25.0f},
      {ECustomerAction::SellItem, 15.0f},
      {ECustomerAction::Leave, 10.0f},
  };
  BehaviorParams.AcceptanceMinMulti = 1.0f;
  BehaviorParams.AcceptanceMaxMulti = 1.0f;

  LastSpawnTime = 0.0f;
  LastPickItemCheckTime = 0.0f;

  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) { ChangeBehaviorParam(ParamValues); };
  Upgradeable.UpgradeFunction = [this](const FName FunctionName, const TArray<FName>& Ids,
                                       const TMap<FName, float>& ParamValues) {
    UpgradeFunction(FunctionName, Ids, ParamValues);
  };
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

  PickingItemIdsMap.Empty();

  TArray<ACustomer*> CustomersToRemove;
  for (ACustomer* Customer : AllCustomers) CustomersToRemove.Add(Customer);
  AllCustomers.Empty();
  for (ACustomer* Customer : CustomersToRemove) Customer->Destroy();
}

void ACustomerAIManager::SpawnUniqueNpcs() {
  TArray<struct FUniqueNpcData> EligibleNpcs = GlobalStaticDataManager->GetEligibleNpcs().FilterByPredicate(
      [this](const auto& Npc) { return !RecentlySpawnedUniqueNpcsMap.Contains(Npc.ID); });
  if (EligibleNpcs.Num() <= 0) return;

  ASpawnPoint* SpawnPoint = GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)[0];
  check(SpawnPoint);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoint->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  FUniqueNpcData UniqueNpcData =
      GetWeightedRandomItem<FUniqueNpcData>(EligibleNpcs, [](const auto& Npc) { return Npc.SpawnWeight; });
  RecentlySpawnedUniqueNpcsMap.Add(UniqueNpcData.ID, ManagerParams.RecentNpcSpawnedKeepTime);

  UE_LOG(LogTemp, Warning, TEXT("Spawning unique npc: %s."), *UniqueNpcData.ID.ToString());
  ACustomer* UniqueCustomer = GetWorld()->SpawnActor<ACustomer>(CustomerClass, SpawnPoint->GetActorLocation(),
                                                                SpawnPoint->GetActorRotation(), SpawnParams);

  UniqueCustomer->SimpleSpriteAnimComponent->IdleSprites = UniqueNpcData.AssetData.IdleSprites;
  UniqueCustomer->SimpleSpriteAnimComponent->WalkSprites = UniqueNpcData.AssetData.WalkSprites;
  UniqueCustomer->SimpleSpriteAnimComponent->Idle();

  UniqueCustomer->CustomerAIComponent->CustomerState = ECustomerState::Browsing;
  UniqueCustomer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

  // ? Use a random dialogue component type?
  UniqueCustomer->DialogueComponent->DialogueArray =
      GlobalStaticDataManager->GetRandomNpcDialogue(UniqueNpcData.DialogueChainIDs);

  UniqueCustomer->CustomerAIComponent->CustomerType = ECustomerType::Unique;
  UniqueCustomer->CustomerAIComponent->Attitude = UniqueNpcData.NegotiationData.Attitude;
  UniqueCustomer->CustomerAIComponent->NegotiationAI->AcceptancePercentage =
      FMath::FRandRange(UniqueNpcData.NegotiationData.AcceptancePercentageRange[0],
                        UniqueNpcData.NegotiationData.AcceptancePercentageRange[1]) /
      100.0f;
  UniqueCustomer->CustomerAIComponent->NegotiationAI->DialoguesMap =
      GlobalStaticDataManager->GetRandomNegDialogueMap(UniqueNpcData.NegotiationData.Attitude);

  const FCustomerPop* CustomerPopData = MarketEconomy->CustomerPops.FindByPredicate(
      [UniqueNpcData](const FCustomerPop& Pop) { return Pop.ID == UniqueNpcData.LinkedPopID; });
  const FPopEconData* PopEconData = MarketEconomy->PopEconDataArray.FindByPredicate(
      [UniqueNpcData](const FPopEconData& Pop) { return Pop.PopID == UniqueNpcData.LinkedPopID; });
  check(CustomerPopData && PopEconData);
  UniqueCustomer->CustomerAIComponent->ItemEconTypes = CustomerPopData->ItemEconTypes;
  UniqueCustomer->CustomerAIComponent->MoneyToSpend = PopEconData->Money / PopEconData->Population;

  AllCustomers.Add(UniqueCustomer);

  // Quest override.
  auto EligibleQuestChains = QuestManager->GetEligibleQuestChains(UniqueNpcData.QuestIDs);
  if (EligibleQuestChains.Num() <= 0) return;

  const FQuestChainData& RandomQuestChainData =
      GetWeightedRandomItem<FQuestChainData>(EligibleQuestChains, [](const auto& Chain) { return Chain.StartChance; });
  if (FMath::FRand() * 100 >= RandomQuestChainData.StartChance) return;

  switch (RandomQuestChainData.CustomerAction) {
    case ECustomerAction::PickItem:
      if (!CustomerPickItem(UniqueCustomer->CustomerAIComponent, [&RandomQuestChainData](const auto& StockItem) {
            return RandomQuestChainData.ActionRelevantIDs.Contains(StockItem.Item->ItemID);
          }))
        return;
      break;
    case ECustomerAction::StockCheck:
      if (!CustomerStockCheck(UniqueCustomer->CustomerAIComponent, [&RandomQuestChainData](const auto& ItemType) {
            return RandomQuestChainData.ActionRelevantIDs.Contains(ItemType.WantedItemTypeID);
          }))
        return;
      break;
    case ECustomerAction::SellItem:
      CustomerSellItem(UniqueCustomer->CustomerAIComponent,
                       Market->GetRandomItem(RandomQuestChainData.ActionRelevantIDs));
      break;
    default: break;
  }

  UniqueCustomer->CustomerAIComponent->CustomerState = ECustomerState::PerformingQuest;

  UniqueCustomer->QuestComponent->ChainID = RandomQuestChainData.ID;
  UniqueCustomer->QuestComponent->CustomerAction = RandomQuestChainData.CustomerAction;
  UniqueCustomer->QuestComponent->QuestOutcomeType = RandomQuestChainData.QuestOutcomeType;

  UniqueCustomer->InteractionComponent->InteractionType = EInteractionType::UniqueNPCQuest;
  UniqueCustomer->DialogueComponent->DialogueArray =
      GlobalStaticDataManager->GetQuestDialogue(RandomQuestChainData.DialogueChainID);

  UniqueCustomer->WidgetComponent->SetVisibility(true, true);
  // UniqueCustomer->WidgetComponent->SetDrawSize(FVector2D(100.0f, 100.0f));
}

void ACustomerAIManager::SpawnCustomers() {
  check(CustomerClass && GlobalStaticDataManager && MarketEconomy && Store);
  if (AllCustomers.Num() >= BehaviorParams.MaxCustomers) return;

  LastSpawnTime = GetWorld()->GetTimeSeconds();

  ASpawnPoint* SpawnPoint = GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)[0];
  check(SpawnPoint);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoint->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  if (ManagerParams.UniqueNpcBaseSpawnChance >= FMath::FRand() * 100) SpawnUniqueNpcs();

  TArray<TTuple<FGenericCustomerData, float>> WeightedCustomers;
  for (const auto& GenericCustomer : GlobalStaticDataManager->GenericCustomersArray) {
    const auto PopData = *MarketEconomy->PopEconDataArray.FindByPredicate(
        [GenericCustomer](const auto& Pop) { return Pop.PopID == GenericCustomer.LinkedPopID; });
    WeightedCustomers.Add({GenericCustomer, PopData.Population * MarketEconomy->GetPopWeightingMulti(PopData)});
  }

  int32 CustomersToSpawn = FMath::RandRange(0, BehaviorParams.MaxCustomers - AllCustomers.Num());
  UE_LOG(LogTemp, Warning, TEXT("Spawning %d customers."), CustomersToSpawn);
  for (int32 i = 0; i < CustomersToSpawn; i++) {
    if (BehaviorParams.CustomerSpawnChance < FMath::FRand() * 100) continue;

    ACustomer* Customer = GetWorld()->SpawnActor<ACustomer>(CustomerClass, SpawnPoint->GetActorLocation(),
                                                            SpawnPoint->GetActorRotation(), SpawnParams);

    const FGenericCustomerData RandomCustomerData =
        GetWeightedRandomItem<TTuple<FGenericCustomerData, float>>(WeightedCustomers, [](const auto& Item) {
          return Item.Value;
        }).Key;

    Customer->SimpleSpriteAnimComponent->IdleSprites = RandomCustomerData.AssetData.IdleSprites;
    Customer->SimpleSpriteAnimComponent->WalkSprites = RandomCustomerData.AssetData.WalkSprites;
    Customer->SimpleSpriteAnimComponent->Idle();

    const auto* CustomerPopData = MarketEconomy->CustomerPops.FindByPredicate(
        [RandomCustomerData](const FCustomerPop& Pop) { return Pop.ID == RandomCustomerData.LinkedPopID; });
    const auto* PopMoneySpendData = MarketEconomy->PopEconDataArray.FindByPredicate(
        [RandomCustomerData](const auto& Pop) { return Pop.PopID == RandomCustomerData.LinkedPopID; });
    check(CustomerPopData && PopMoneySpendData);

    Customer->CustomerAIComponent->CustomerState = ECustomerState::Browsing;
    Customer->CustomerAIComponent->CustomerType = ECustomerType::Generic;
    Customer->CustomerAIComponent->ItemEconTypes = CustomerPopData->ItemEconTypes;
    Customer->CustomerAIComponent->MoneyToSpend = PopMoneySpendData->Money / PopMoneySpendData->Population;
    Customer->CustomerAIComponent->Attitude = RandomCustomerData.InitAttitude;

    Customer->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    Customer->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomCustomerDialogue();

    // ? Maybe have two arrays for customers ai and customer interaction components?
    AllCustomers.Add(Customer);
  }
}

void ACustomerAIManager::PerformCustomerAILoop() {
  check(Store);
  LastPickItemCheckTime = GetWorld()->GetTimeSeconds();

  TArray<ACustomer*> CustomersToRemove;

  UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
  check(NavSystem);

  for (ACustomer* Customer : AllCustomers) {
    switch (Customer->CustomerAIComponent->CustomerState) {
      case (ECustomerState::Browsing): {
        if (PickingItemIdsMap.Num() < ManagerParams.MaxCustomersPickingAtOnce) {
          if (FMath::FRand() * 100 < BehaviorParams.PerformActionChance) {
            CustomerPerformAction(Customer);
            // Stop movement.
            break;
          }
        }

        MoveCustomerRandom(NavSystem, Customer);
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
      case (ECustomerState::PerformingQuest): {
        break;
      }
      default: checkNoEntry();
    }
  }

  for (ACustomer* Customer : CustomersToRemove) {
    AllCustomers.RemoveSingleSwap(Customer);

    // ? Call function somewhere to remove the picked item on successful negotiation?
    if (auto ItemID = PickingItemIdsMap.FindKey(Customer->CustomerAIComponent->CustomerID))
      PickingItemIdsMap.Remove(*ItemID);

    if (Customer->CustomerAIComponent->NegotiationAI) {
      Customer->CustomerAIComponent->NegotiationAI->RelevantItem = nullptr;
      Customer->CustomerAIComponent->NegotiationAI = nullptr;
    }
    Customer->Destroy();
  }
}

void ACustomerAIManager::MoveCustomerRandom(UNavigationSystemV1* NavSystem, ACustomer* Customer) {
  check(NavSystem && Customer);

  AAIController* OwnerAIController = Customer->GetController<AAIController>();
  if (!OwnerAIController) return;
  if (OwnerAIController->GetMoveStatus() != EPathFollowingStatus::Idle) return;

  FNavLocation RandomLocation;
  NavSystem->GetRandomReachablePointInRadius(Customer->GetActorLocation(), float(300.0f), RandomLocation);
  Customer->GetCharacterMovement()->MaxFlySpeed = FMath::FRandRange(50.0f, 250.0f);

  // ? Do we only need to bind it once?
  OwnerAIController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
  OwnerAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
      Customer->SimpleSpriteAnimComponent, &USimpleSpriteAnimComponent::MoveCompleted);

  FVector Direction = (RandomLocation.Location - Customer->GetActorLocation()).GetSafeNormal();
  ESimpleSpriteDirection DirectionEnum = ESimpleSpriteDirection::Down;
  if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
    DirectionEnum = Direction.X > 0 ? ESimpleSpriteDirection::Right : ESimpleSpriteDirection::Left;
  else DirectionEnum = Direction.Y > 0 ? ESimpleSpriteDirection::Down : ESimpleSpriteDirection::Up;
  Customer->SimpleSpriteAnimComponent->Walk(DirectionEnum);

  OwnerAIController->MoveToLocation(RandomLocation, 1.0f, false, true, true, false);
}

void ACustomerAIManager::CustomerPerformAction(class ACustomer* Customer) {
  check(Customer);

  TMap<ECustomerAction, float> ActionWeights = BehaviorParams.ActionWeights;
  if (Store->StoreStockItems.Num() < 5.0f)
    ActionWeights[ECustomerAction::PickItem] -= (5.0f - Store->StoreStockItems.Num()) * 5.0f;

  ECustomerAction RandomAction =
      GetWeightedRandomItem<TTuple<ECustomerAction, float>>(ActionWeights.Array(), [](const auto& Action) {
        return Action.Value;
      }).Key;

  ECustomerAction Action = Customer->CustomerAIComponent->CustomerAction != ECustomerAction::None
                               ? Customer->CustomerAIComponent->CustomerAction
                               : RandomAction;
  switch (Action) {
    case (ECustomerAction::PickItem):
      if (CustomerPickItem(Customer->CustomerAIComponent)) MakeCustomerNegotiable(Customer);
      break;
    case (ECustomerAction::StockCheck):
      if (CustomerStockCheck(Customer->CustomerAIComponent)) MakeCustomerNegotiable(Customer);
      break;
    case (ECustomerAction::SellItem):
      CustomerSellItem(Customer->CustomerAIComponent);
      MakeCustomerNegotiable(Customer);
      break;
    case (ECustomerAction::Leave): {
      Customer->CustomerAIComponent->CustomerState = ECustomerState::Leaving;
      break;
    }
    default: break;
  }
}

// ? Use item ids to pick items?
bool ACustomerAIManager::CustomerPickItem(UCustomerAIComponent* CustomerAI,
                                          std::function<bool(const FStockItem& StockItem)> FilterFunc) {
  if (Store->StoreStockItems.Num() <= 0 || PickingItemIdsMap.Num() >= Store->StoreStockItems.Num()) return false;

  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !PickingItemIdsMap.Contains(StockItem.Item->UniqueItemID); });
  auto RelevantItems = FilterFunc ? NonPickedItems.FilterByPredicate(FilterFunc)
                                  : NonPickedItems.FilterByPredicate([CustomerAI](const FStockItem& StockItem) {
                                      return CustomerAI->ItemEconTypes.Contains(StockItem.Item->ItemEconType) &&
                                             CustomerAI->MoneyToSpend >= StockItem.Item->PriceData.BoughtAt;
                                    });
  if (RelevantItems.Num() <= 0) return false;

  const FStockItem& StockItem = GetWeightedRandomItem<FStockItem>(
      RelevantItems, [](const auto& StockItem) { return StockItem.DisplayStats.PickRateWeightModifier; });
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAI->RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAI->StockDisplayInventory = StockItem.BelongingStockInventoryC;

  PickingItemIdsMap.Add(StockItem.Item->UniqueItemID, CustomerAI->CustomerID);
  return true;
}

// Note: Agnostic about if player has item type or not.
bool ACustomerAIManager::CustomerStockCheck(UCustomerAIComponent* CustomerAI,
                                            std::function<bool(const FWantedItemType& ItemType)> FilterFunc) {
  auto ValidItemTypes = FilterFunc ? GlobalStaticDataManager->WantedItemTypesArray.FilterByPredicate(FilterFunc)
                                   : GlobalStaticDataManager->WantedItemTypesArray.FilterByPredicate(
                                         [CustomerAI](const FWantedItemType& ItemType) {
                                           return CustomerAI->ItemEconTypes.Contains(ItemType.ItemEconType);
                                         });
  if (ValidItemTypes.Num() <= 0) return false;

  const FWantedItemType& WantedItemType = ValidItemTypes[FMath::RandRange(0, ValidItemTypes.Num() - 1)];
  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::StockCheck;
  CustomerAI->NegotiationAI->WantedItemType = WantedItemType;
  return true;
}

void ACustomerAIManager::CustomerSellItem(UCustomerAIComponent* CustomerAI, UItemBase* HasItem) {
  UItemBase* Item = HasItem ? HasItem : Market->GetNewRandomItems(1, {}, {}, CustomerAI->ItemEconTypes)[0];
  check(Item);

  CustomerAI->NegotiationAI->RequestType = ECustomerRequestType::SellItem;
  CustomerAI->NegotiationAI->RelevantItem = Item;
}

void ACustomerAIManager::MakeCustomerNegotiable(class ACustomer* Customer) {
  UCustomerAIComponent* CustomerAI = Customer->CustomerAIComponent;

  Customer->InteractionComponent->InteractionType = EInteractionType::WaitingCustomer;
  Customer->WidgetComponent->SetVisibility(true, true);
  // Customer->WidgetComponent->SetDrawSize(FVector2D(100.0f, 100.0f));

  CustomerAI->CustomerState = ECustomerState::Requesting;

  CustomerAI->NegotiationAI->DialoguesMap = GlobalStaticDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude);

  if (CustomerAI->CustomerType == ECustomerType::Unique) return;

  float AcceptanceMin = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 0.05f
                        : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 0.00f
                                                                             : 0.05f;
  float AcceptanceMax = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 0.25f
                        : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 0.10f
                                                                             : 0.15f;
  CustomerAI->NegotiationAI->AcceptancePercentage = FMath::FRandRange(
      AcceptanceMin * BehaviorParams.AcceptanceMinMulti, AcceptanceMax * BehaviorParams.AcceptanceMaxMulti);
}

void ACustomerAIManager::TickDaysTimedVars() {
  TArray<FName> NpcsToRemove;
  for (auto& Pair : RecentlySpawnedUniqueNpcsMap)
    if (Pair.Value <= 1) NpcsToRemove.Add(Pair.Key);
    else Pair.Value--;

  for (const auto& NpcId : NpcsToRemove) RecentlySpawnedUniqueNpcsMap.Remove(NpcId);
}

void ACustomerAIManager::ChangeBehaviorParam(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    auto StructProp = CastField<FStructProperty>(this->GetClass()->FindPropertyByName("BehaviorParams"));
    auto StructPtr = StructProp->ContainerPtrToValuePtr<void>(this);
    SetStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}

void ACustomerAIManager::UpgradeFunction(FName FunctionName,
                                         const TArray<FName>& Ids,
                                         const TMap<FName, float>& ParamValues) {
  check(FunctionName == "ChangeActionWeights");  // Temp: only one function.

  for (const auto& ParamPair : ParamValues) {
    ECustomerAction Action = ECustomerAction::None;
    for (auto A : TEnumRange<ECustomerAction>())
      if (UEnum::GetDisplayValueAsText(A).ToString() == ParamPair.Key) {
        Action = A;
        break;
      }
    // UE_LOG(LogTemp, Warning, TEXT("ParamPair: %s"), *ParamPair.Key.ToString());
    check(Action != ECustomerAction::None);

    BehaviorParams.ActionWeights[Action] = FMath::Max(BehaviorParams.ActionWeights[Action], 1.0f) * ParamPair.Value;
  }
}