#include "CustomerAIManager.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "CustomerAIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "store_playground/WorldObject/Customer.h"
#include "store_playground/WorldObject/CustomerPC.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Ability/AbilityManager.h"
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
#include "Navigation/PathFollowingComponent.h"
#include "Components/WidgetComponent.h"
#include "PaperFlipbookComponent.h"

ACustomerAIManager::ACustomerAIManager() {
  PrimaryActorTick.bCanEverTick = true;
  // PrimaryActorTick.TickInterval = 1.0f;

  ManagerParams.bSpawnCustomers = false;
  ManagerParams.CustomerSpawnInterval = 5.0f;
  ManagerParams.UniqueNpcBaseSpawnChance = 10.0f;
  ManagerParams.RecentNpcSpawnedKeepTime = 2.0f;
  ManagerParams.UNpcMaxSpawnPerDay = 1;
  ManagerParams.PickItemFrequency = 5.0f;

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
  BehaviorParams.CustomerWaitingTime = 10.0f;

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

  TArray<ACustomerPC*> CustomersToRemove;
  for (ACustomerPC* Customer : AllCustomers) CustomersToRemove.Add(Customer);
  for (ACustomerPC* Customer : ExitingCustomers) CustomersToRemove.Add(Customer);
  AllCustomers.Empty();
  ExitingCustomers.Empty();

  for (ACustomerPC* Customer : CustomersToRemove) Customer->Destroy();
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
  ACustomerPC* UniqueCustomer = GetWorld()->SpawnActor<ACustomerPC>(CustomerClass, SpawnPoint->GetActorLocation(),
                                                                    SpawnPoint->GetActorRotation(), SpawnParams);

  UniqueCustomer->SimpleSpriteAnimComponent->IdleSprites = UniqueNpcData.AssetData.IdleSprites;
  UniqueCustomer->SimpleSpriteAnimComponent->WalkSprites = UniqueNpcData.AssetData.WalkSprites;
  UniqueCustomer->SimpleSpriteAnimComponent->Idle();

  UniqueCustomer->CustomerAIComponent->CustomerState = ECustomerState::Browsing;
  UniqueCustomer->InteractionComponent->InteractionType = EInteractionType::Customer;
  UniqueCustomer->DialogueComponent->SpeakerName = UniqueNpcData.NpcName;

  // ? Use a random dialogue component type?
  UniqueCustomer->DialogueComponent->DialogueArray =
      GlobalStaticDataManager->GetRandomNpcDialogue(UniqueNpcData.DialogueChainIDs, {"Level.Store"});

  UniqueCustomer->CustomerAIComponent->CustomerName = UniqueNpcData.NpcName;
  UniqueCustomer->CustomerAIComponent->CustomerType = ECustomerType::Unique;
  UniqueCustomer->CustomerAIComponent->Attitude = UniqueNpcData.NegotiationData.Attitude;
  UniqueCustomer->CustomerAIComponent->Tags = UniqueNpcData.NegotiationData.Tags;

  const FCustomerPop* CustomerPopData = MarketEconomy->CustomerPops.FindByPredicate(
      [UniqueNpcData](const FCustomerPop& Pop) { return Pop.ID == UniqueNpcData.LinkedPopID; });
  const FPopEconData* PopEconData = MarketEconomy->PopEconDataArray.FindByPredicate(
      [UniqueNpcData](const FPopEconData& Pop) { return Pop.PopID == UniqueNpcData.LinkedPopID; });
  check(CustomerPopData && PopEconData);
  UniqueCustomer->CustomerAIComponent->ItemEconTypes = CustomerPopData->ItemEconTypes;
  UniqueCustomer->CustomerAIComponent->AvailableMoney =
      (PopEconData->Money / PopEconData->Population) * BehaviorParams.AvailableMoneyMulti;

  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.CustomerName = UniqueNpcData.NpcName;
  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.DialoguesMap =
      GlobalStaticDataManager->GetRandomNegDialogueMap(
          UniqueNpcData.NegotiationData.Attitude, [&](const FDialogueData& Dialogue) {
            return Dialogue.DialogueTags.IsEmpty() ||
                   Dialogue.DialogueTags.HasAny(UniqueCustomer->CustomerAIComponent->Tags);
          });

  float MoneyToSpend = UniqueCustomer->CustomerAIComponent->AvailableMoney;
  float AcceptancePercentage = FMath::FRandRange(UniqueNpcData.NegotiationData.AcceptancePercentageRange[0],
                                                 UniqueNpcData.NegotiationData.AcceptancePercentageRange[1]);

  for (const FNegotiationSkill& Skill : AbilityManager->ActiveNegotiationSkills)
    if (Skill.EffectType == ECustomerAIEffect::AcceptancePercentage) AcceptancePercentage *= Skill.Multi;

  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.MoneyToSpend = MoneyToSpend;
  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.AcceptancePercentage = AcceptancePercentage;
  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.AcceptanceFalloffMulti =
      ManagerParams.AttitudeBaseAcceptFalloffMultiMap[UniqueCustomer->CustomerAIComponent->Attitude] *
      BehaviorParams.AcceptanceFalloffMulti;
  UniqueCustomer->CustomerAIComponent->NegotiationAIDetails.HagglingCount =
      UniqueNpcData.NegotiationData.MaxHagglingCount;

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

  UniqueCustomer->CustomerAIComponent->CustomerState = ECustomerState::RequestingQuest;

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
  LastSpawnTime = GetWorld()->GetTimeSeconds();

  float FilledStockPercent =
      Store->StockDisplayCount > 0
          ? FMath::Clamp(Store->StoreStockItems.Num() / (float)Store->StockDisplayCount, 0.0f, 1.0f)
          : 0.0f;
  int32 MaxCustomers =
      FMath::RoundToInt(BehaviorParams.MaxCustomers * FMath::Clamp(FilledStockPercent * 1.5f, 0.75f, 1.25f));
  if (AllCustomers.Num() >= MaxCustomers) return;

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
    const auto PopCustomerData = *MarketEconomy->CustomerPops.FindByPredicate(
        [GenericCustomer](const auto& CPop) { return CPop.ID == GenericCustomer.LinkedPopID; });
    const auto PopData = *MarketEconomy->PopEconDataArray.FindByPredicate(
        [GenericCustomer](const auto& Pop) { return Pop.PopID == GenericCustomer.LinkedPopID; });

    float PopWeightingMulti = MarketEconomy->GetPopWeightingMulti(PopData) *
                              BehaviorParams.PopTypeMultis[PopCustomerData.PopType] *
                              BehaviorParams.PopWealthTypeMultis[PopCustomerData.WealthType];
    WeightedCustomers.Add({GenericCustomer, PopData.Population * PopWeightingMulti});
  }

  int32 MaxCustomersToSpawn = FMath::Min(MaxCustomers - AllCustomers.Num(), ManagerParams.MaxSpawnCustomersInOneGo);
  int32 CustomersToSpawn = FMath::RandRange(0, MaxCustomersToSpawn);
  for (int32 i = 0; i < CustomersToSpawn; i++) {
    float SpawnChance = BehaviorParams.CustomerSpawnChance * FMath::Clamp(FilledStockPercent * 1.5f, 0.75f, 1.25f);
    if (SpawnChance < FMath::FRand() * 100) continue;

    ACustomerPC* Customer = GetWorld()->SpawnActor<ACustomerPC>(
        CustomerClass,
        SpawnPoint->GetActorLocation() +
            FVector(FMath::FRandRange(-50.0f, 50.0f), FMath::FRandRange(-50.0f, 50.0f), 0.0f),
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

    Customer->CustomerAIComponent->CustomerName = RandomCustomerData.CustomerName;
    Customer->CustomerAIComponent->CustomerState = ECustomerState::Browsing;
    Customer->CustomerAIComponent->CustomerType = ECustomerType::Generic;
    Customer->CustomerAIComponent->ItemEconTypes = CustomerPopData->ItemEconTypes;
    Customer->CustomerAIComponent->Attitude = RandomCustomerData.InitAttitude;
    Customer->CustomerAIComponent->Tags = RandomCustomerData.Tags;
    Customer->CustomerAIComponent->AvailableMoney =
        (PopMoneySpendData->Money / PopMoneySpendData->Population) * BehaviorParams.AvailableMoneyMulti;

    Customer->InteractionComponent->InteractionType = EInteractionType::Customer;

    Customer->DialogueComponent->SpeakerName = RandomCustomerData.CustomerName;
    Customer->DialogueComponent->DialogueArray =
        GlobalStaticDataManager->GetRandomCustomerDialogue([&](const FDialogueData& Dialogue) {
          return Dialogue.DialogueTags.IsEmpty() || Dialogue.DialogueTags.HasAny(Customer->CustomerAIComponent->Tags);
        });

    // ? Maybe have two arrays for customers ai and customer interaction components?
    AllCustomers.Add(Customer);
  }
}

void ACustomerAIManager::PerformCustomerAILoop() {
  LastPickItemCheckTime = GetWorld()->GetTimeSeconds();

  UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
  check(NavSystem);

  TMap<ECustomerAction, float> ActionWeights = BehaviorParams.ActionWeights;
  if (Store->StoreStockItems.Num() < 5.0f)
    ActionWeights[ECustomerAction::PickItem] -= (5.0f - Store->StoreStockItems.Num()) * 5.0f;

  TArray<ACustomerPC*> CustomersToExit;
  for (ACustomerPC* Customer : AllCustomers) {
    switch (Customer->CustomerAIComponent->CustomerState) {
      case (ECustomerState::Browsing): {
        if (FMath::FRand() * 100 < BehaviorParams.PerformActionChance) {
          CustomerPerformAction(Customer, ActionWeights);
          break;
        }

        if (FMath::FRand() < 0.5f) MoveCustomerRandom(NavSystem, Customer);
        break;
      }
      case (ECustomerState::BrowsingTalking): {
        break;
      }
      case (ECustomerState::Requesting): {
        if (GetWorld()->TimeSince(Customer->CustomerAIComponent->RequestingTime) > BehaviorParams.CustomerWaitingTime) {
          Customer->WidgetComponent->SetVisibility(false, true);
          Customer->InteractionComponent->InteractionType = EInteractionType::None;
          Customer->CustomerAIComponent->CustomerState = ECustomerState::Leaving;
          CustomersToExit.Add(Customer);
        }
        break;
      }
      case (ECustomerState::RequestingQuest): {
        if (FMath::FRand() < 0.5f) MoveCustomerRandom(NavSystem, Customer);
        break;
      }
      case (ECustomerState::PerformingQuest): {
        break;
      }
      case (ECustomerState::Negotiating): {
        break;
      }
      case (ECustomerState::Leaving): {
        CustomersToExit.Add(Customer);
        break;
      }
      default: checkNoEntry();
    }
  }

  TArray<ACustomerPC*> CustomersToDestroy;
  for (ACustomerPC* Customer : ExitingCustomers) {
    if (Customer->GetController<AAIController>()->GetMoveStatus() != EPathFollowingStatus::Idle) continue;

    check(Customer->CustomerAIComponent->CustomerState == ECustomerState::Leaving);

    CustomersToDestroy.Add(Customer);

    if (Customer->CustomerAIComponent->NegotiationAIDetails.RelevantItem)
      Customer->CustomerAIComponent->NegotiationAIDetails.RelevantItem = nullptr;
  }
  for (ACustomerPC* Customer : CustomersToDestroy) {
    ExitingCustomers.RemoveSingleSwap(Customer);
    Customer->Destroy();
  }

  if (CustomersToExit.Num() <= 0) return;
  ASpawnPoint* SpawnPoint = GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)[0];  // Spawn point is exit.
  check(SpawnPoint);
  FVector ExitLocation = SpawnPoint->GetActorLocation();
  for (ACustomerPC* Customer : CustomersToExit) {
    MoveCustomerToExit(NavSystem, Customer, ExitLocation);

    ExitingCustomers.Add(Customer);
    AllCustomers.RemoveSingleSwap(Customer);

    // ? Call function somewhere to remove the picked item on successful negotiation?
    if (auto ItemID = PickingItemIdsMap.FindKey(Customer->CustomerAIComponent->CustomerAIID))
      PickingItemIdsMap.Remove(*ItemID);
  }
}

void ACustomerAIManager::MoveCustomerRandom(UNavigationSystemV1* NavSystem, ACustomerPC* Customer) {
  check(NavSystem && Customer);

  AAIController* OwnerAIController = Customer->GetController<AAIController>();
  if (!OwnerAIController) return;
  if (OwnerAIController->GetMoveStatus() != EPathFollowingStatus::Idle) return;

  FNavLocation RandomLocation;
  NavSystem->GetRandomReachablePointInRadius(Customer->GetActorLocation(), float(500.0f), RandomLocation);
  Customer->GetCharacterMovement()->MaxFlySpeed = FMath::FRandRange(75.0f, 300.0f);

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

  OwnerAIController->MoveToLocation(RandomLocation, 1.0f, false, true, false, false);
}
void ACustomerAIManager::MoveCustomerToExit(UNavigationSystemV1* NavSystem,
                                            ACustomerPC* Customer,
                                            FVector ExitLocation) {
  check(NavSystem && Customer);

  AAIController* OwnerAIController = Customer->GetController<AAIController>();
  check(OwnerAIController);
  check(OwnerAIController->GetMoveStatus() == EPathFollowingStatus::Idle);

  FVector Direction = (ExitLocation - Customer->GetActorLocation()).GetSafeNormal();
  ESimpleSpriteDirection DirectionEnum = ESimpleSpriteDirection::Down;
  if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
    DirectionEnum = Direction.X > 0 ? ESimpleSpriteDirection::Right : ESimpleSpriteDirection::Left;
  else DirectionEnum = Direction.Y > 0 ? ESimpleSpriteDirection::Down : ESimpleSpriteDirection::Up;
  Customer->SimpleSpriteAnimComponent->Walk(DirectionEnum);

  OwnerAIController->MoveToLocation(ExitLocation, 1.0f, false, true, false, false);
}

void ACustomerAIManager::CustomerPerformAction(class ACustomerPC* Customer,
                                               TMap<ECustomerAction, float> ActionWeights) {
  ECustomerAction RandomAction =
      GetWeightedRandomItem<TTuple<ECustomerAction, float>>(ActionWeights.Array(), [](const auto& Action) {
        return Action.Value;
      }).Key;
  ECustomerAction Action = Customer->CustomerAIComponent->CustomerAction != ECustomerAction::None
                               ? Customer->CustomerAIComponent->CustomerAction
                               : RandomAction;
  // ? Fallthrough?
  switch (Action) {
    case (ECustomerAction::PickItem):
      if (CustomerPickItem(Customer->CustomerAIComponent)) MakeCustomerNegotiable(Customer);
      break;
    case (ECustomerAction::StockCheck):
      if (CustomerStockCheck(Customer->CustomerAIComponent)) MakeCustomerNegotiable(Customer);
      break;
    case (ECustomerAction::SellItem):
      if (CustomerSellItem(Customer->CustomerAIComponent)) MakeCustomerNegotiable(Customer);

      break;
    case (ECustomerAction::Leave): {
      AAIController* OwnerAIController = Customer->GetController<AAIController>();
      if (OwnerAIController) OwnerAIController->StopMovement();

      Customer->InteractionComponent->InteractionType = EInteractionType::None;
      Customer->CustomerAIComponent->CustomerState = ECustomerState::Leaving;
      break;
    }
    default: break;
  }
}

// ? Use item ids to pick items?
auto ACustomerAIManager::CustomerPickItem(UCustomerAIComponent* CustomerAI,
                                          std::function<bool(const FStockItem& StockItem)> FilterFunc) -> bool {
  if (Store->StoreStockItems.Num() <= 0 || PickingItemIdsMap.Num() >= Store->StoreStockItems.Num()) return false;

  TArray<FStockItem> NonPickedItems = Store->StoreStockItems.FilterByPredicate(
      [this](const FStockItem& StockItem) { return !PickingItemIdsMap.Contains(StockItem.Item->UniqueItemID); });
  auto RelevantItems = FilterFunc ? NonPickedItems.FilterByPredicate(FilterFunc)
                                  : NonPickedItems.FilterByPredicate([CustomerAI](const FStockItem& StockItem) {
                                      return CustomerAI->ItemEconTypes.Contains(StockItem.Item->ItemEconType) &&
                                             CustomerAI->AvailableMoney >= StockItem.Item->PlayerPriceData.BoughtAt;
                                    });
  if (RelevantItems.Num() <= 0) return false;

  const FStockItem& StockItem = GetWeightedRandomItem<FStockItem>(RelevantItems, [](const auto& StockItem) {
    return StockItem.StockDisplayComponent->DisplayStats.PickRateWeightModifier;
  });
  CustomerAI->NegotiationAIDetails.RequestType = ECustomerRequestType::BuyStockItem;
  CustomerAI->NegotiationAIDetails.RelevantItem = StockItem.Item;
  CustomerAI->NegotiationAIDetails.StockDisplayInventory = StockItem.BelongingStockInventoryC;

  PickingItemIdsMap.Add(StockItem.Item->UniqueItemID, CustomerAI->CustomerAIID);
  return true;
}

// Note: Agnostic about if player has item type or not.
auto ACustomerAIManager::CustomerStockCheck(UCustomerAIComponent* CustomerAI,
                                            std::function<bool(const FWantedItemType& ItemType)> FilterFunc) -> bool {
  auto ValidItemTypes = FilterFunc ? GlobalStaticDataManager->WantedItemTypesArray.FilterByPredicate(FilterFunc)
                                   : GlobalStaticDataManager->WantedItemTypesArray.FilterByPredicate(
                                         [CustomerAI](const FWantedItemType& ItemType) {
                                           return CustomerAI->ItemEconTypes.Contains(ItemType.ItemEconType);
                                         });
  if (ValidItemTypes.Num() <= 0) return false;

  const FWantedItemType& WantedItemType = ValidItemTypes[FMath::RandRange(0, ValidItemTypes.Num() - 1)];
  CustomerAI->NegotiationAIDetails.RequestType = ECustomerRequestType::StockCheck;
  CustomerAI->NegotiationAIDetails.WantedItemType = WantedItemType;
  return true;
}

auto ACustomerAIManager::CustomerSellItem(UCustomerAIComponent* CustomerAI, UItemBase* HasItem) -> bool {
  if (HasItem) {
    CustomerAI->NegotiationAIDetails.RequestType = ECustomerRequestType::SellItem;
    CustomerAI->NegotiationAIDetails.RelevantItem = HasItem;
    return true;
  }

  auto RandomItems =
      Market->GetNewRandomItems(1, {}, {}, CustomerAI->ItemEconTypes, [this, CustomerAI](const FName& ItemId) {
        return MarketEconomy->GetMarketPrice(Market->GetItem(ItemId)->ItemID) <= CustomerAI->AvailableMoney;
      });
  if (RandomItems.Num() <= 0) return false;

  UItemBase* Item = RandomItems[0];
  // if (MarketEconomy->GetMarketPrice(Item->ItemID) > Store->Money) return false;

  CustomerAI->NegotiationAIDetails.RequestType = ECustomerRequestType::SellItem;
  CustomerAI->NegotiationAIDetails.RelevantItem = Item;
  return true;
}

void ACustomerAIManager::MakeCustomerNegotiable(class ACustomerPC* Customer) {
  AAIController* OwnerAIController = Customer->GetController<AAIController>();
  if (OwnerAIController) OwnerAIController->StopMovement();

  UCustomerAIComponent* CustomerAI = Customer->CustomerAIComponent;
  CustomerAI->RequestingTime = GetWorld()->GetTimeSeconds();

  Customer->InteractionComponent->InteractionType = EInteractionType::WaitingCustomer;
  Customer->ShowWidget();

  CustomerAI->CustomerState = ECustomerState::Requesting;

  CustomerAI->NegotiationAIDetails.CustomerName = CustomerAI->CustomerName;
  CustomerAI->NegotiationAIDetails.DialoguesMap =
      GlobalStaticDataManager->GetRandomNegDialogueMap(CustomerAI->Attitude, [&](const FDialogueData& Dialogue) {
        return Dialogue.DialogueTags.IsEmpty() || Dialogue.DialogueTags.HasAny(Customer->CustomerAIComponent->Tags);
      });

  if (CustomerAI->CustomerType == ECustomerType::Unique) return;

  float MoneyToSpend = CustomerAI->AvailableMoney;
  if (CustomerAI->Attitude == ECustomerAttitude::Friendly) MoneyToSpend *= FMath::FRandRange(1.0f, 1.5);
  else if (CustomerAI->Attitude == ECustomerAttitude::Hostile) MoneyToSpend *= FMath::FRandRange(0.5f, 1.0f);
  // float AcceptanceMin = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 5.0f
  //                       : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 0.0f
  //                                                                            : 2.5f;
  // float AcceptanceMax = CustomerAI->Attitude == ECustomerAttitude::Friendly  ? 25.0f
  //                       : CustomerAI->Attitude == ECustomerAttitude::Hostile ? 10.0f
  //                                                                            : 15.0f;
  float AcceptanceMin =
      ManagerParams.AttitudeBaseAcceptMinMap[CustomerAI->Attitude] * BehaviorParams.AcceptanceMinMulti;
  float AcceptanceMax =
      ManagerParams.AttitudeBaseAcceptMaxMap[CustomerAI->Attitude] * BehaviorParams.AcceptanceMaxMulti;
  float AcceptancePercentage = FMath::FRandRange(AcceptanceMin, AcceptanceMax);
  float AcceptanceFalloffMulti =
      ManagerParams.AttitudeBaseAcceptFalloffMultiMap[CustomerAI->Attitude] * BehaviorParams.AcceptanceFalloffMulti;
  int32 HagglingCount = FMath::Max(BehaviorParams.InitHagglingCount, 1);

  for (const FNegotiationSkill& Skill : AbilityManager->ActiveNegotiationSkills)
    if (Skill.EffectType == ECustomerAIEffect::AcceptancePercentage) AcceptancePercentage *= Skill.Multi;

  CustomerAI->NegotiationAIDetails.MoneyToSpend = MoneyToSpend;
  CustomerAI->NegotiationAIDetails.AcceptancePercentage = AcceptancePercentage;
  CustomerAI->NegotiationAIDetails.AcceptanceFalloffMulti = AcceptanceFalloffMulti;
  CustomerAI->NegotiationAIDetails.HagglingCount = HagglingCount;
}

auto ACustomerAIManager::ConsiderOffer(UCustomerAIComponent* CustomerAI,
                                       const UItemBase* Item,
                                       float LastOfferedPrice,
                                       float PlayerOfferedPrice) const -> FOfferResponse {
  check(CustomerAI && Item);

  FNegotiationAIDetails NegotiationAIDetails = CustomerAI->NegotiationAIDetails;

  // ? Add error margin to prices / round to nearest int?
  bool bNpcBuying = NegotiationAIDetails.RequestType == ECustomerRequestType::BuyStockItem ||
                    NegotiationAIDetails.RequestType == ECustomerRequestType::StockCheck;

  UE_LOG(LogTemp, Log, TEXT("ConsiderOffer: NPCBuying: %d, LastOfferedPrice: %f, PlayerOfferedPrice: %f"), bNpcBuying,
         LastOfferedPrice, PlayerOfferedPrice);
  if ((bNpcBuying && PlayerOfferedPrice <= LastOfferedPrice) || (!bNpcBuying && PlayerOfferedPrice >= LastOfferedPrice))
    return {true, 0, NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::Accept].Dialogues,
            NegotiationAIDetails.CustomerName};

  NegotiationAIDetails.HagglingCount--;
  if (NegotiationAIDetails.HagglingCount < 0) {
    return {false, 0, NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::Reject].Dialogues,
            NegotiationAIDetails.CustomerName};
  }

  float MarketPrice = MarketEconomy->GetMarketPrice(Item->ItemID);
  float AcceptanceMaxPrice = MarketPrice * (bNpcBuying ? (1.0f + NegotiationAIDetails.AcceptancePercentage / 100.0f)
                                                       : (1.0f - NegotiationAIDetails.AcceptancePercentage / 100.0f));
  float AcceptanceMinPrice = bNpcBuying ? (MarketPrice > LastOfferedPrice ? MarketPrice : LastOfferedPrice)
                                        : (MarketPrice < LastOfferedPrice ? MarketPrice : LastOfferedPrice);
  UE_LOG(LogTemp, Log, TEXT("ConsiderOffer: MarketPrice: %f, AcceptanceMaxPrice: %f, AcceptanceMinPrice: %f"),
         MarketPrice, AcceptanceMaxPrice, AcceptanceMinPrice);
  if (bNpcBuying && PlayerOfferedPrice > AcceptanceMaxPrice || !bNpcBuying && PlayerOfferedPrice < AcceptanceMaxPrice)
    return {false, 0,
            bNpcBuying ? NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::BuyItemTooHigh].Dialogues
                       : NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::SellItemTooLow].Dialogues,
            NegotiationAIDetails.CustomerName};

  float OfferedPercentBetween = (PlayerOfferedPrice - AcceptanceMinPrice) / (AcceptanceMaxPrice - AcceptanceMinPrice);
  float AcceptanceChance = 1.0f - (OfferedPercentBetween * NegotiationAIDetails.AcceptanceFalloffMulti);

  AcceptanceChance = AcceptanceChance > 0.95f ? 1.0f : AcceptanceChance;  // If close enough, always accept.
  UE_LOG(LogTemp, Log, TEXT("ConsiderOffer: OfferedPercentBetween: %f, AcceptanceChance: %f"), OfferedPercentBetween,
         AcceptanceChance);
  if (FMath::FRand() < AcceptanceChance)
    return {true, 0, NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::Accept].Dialogues,
            NegotiationAIDetails.CustomerName};

  NegotiationAIDetails.AcceptancePercentage *= BehaviorParams.PostHagglingAcceptanceMulti;
  float AdjustedPercent = bNpcBuying ? FMath::Min((LastOfferedPrice / MarketPrice) + FMath::FRandRange(0.01f, 0.05f),
                                                  1.0 + NegotiationAIDetails.AcceptancePercentage / 100.0f)
                                     : FMath::Max((LastOfferedPrice / MarketPrice) - FMath::FRandRange(0.01f, 0.05f),
                                                  1.0 - NegotiationAIDetails.AcceptancePercentage / 100.0f);

  UE_LOG(LogTemp, Log, TEXT("ConsiderOffer: AdjustedPercent: %f, AcceptancePercentage: %f"), AdjustedPercent,
         NegotiationAIDetails.AcceptancePercentage);
  return {false, MarketPrice * AdjustedPercent,
          (bNpcBuying ? NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::BuyItemClose].Dialogues
                      : NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::SellItemClose].Dialogues),
          NegotiationAIDetails.CustomerName};
}

auto ACustomerAIManager::ConsiderStockCheck(const UCustomerAIComponent* CustomerAI,
                                            const UItemBase* Item) const -> FOfferResponse {
  check(CustomerAI && Item);

  FNegotiationAIDetails NegotiationAIDetails = CustomerAI->NegotiationAIDetails;
  float MarketPrice = MarketEconomy->GetMarketPrice(Item->ItemID);
  if (Item->ItemType == NegotiationAIDetails.WantedItemType.ItemType &&
      Item->ItemEconType == NegotiationAIDetails.WantedItemType.ItemEconType &&
      MarketPrice <= NegotiationAIDetails.MoneyToSpend)
    return {true, 0, NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::StockCheckAccept].Dialogues,
            NegotiationAIDetails.CustomerName};

  return {false, 0, NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::StockCheckReject].Dialogues,
          NegotiationAIDetails.CustomerName};
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
    AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}

void ACustomerAIManager::UpgradeFunction(FName FunctionName,
                                         const TArray<FName>& Ids,
                                         const TMap<FName, float>& ParamValues) {
  if (FunctionName == "ChangeActionWeights") ChangeActionWeights(ParamValues);
  else if (FunctionName == "ChangePopTypeMultis") ChangePopTypeMultis(ParamValues);
  else if (FunctionName == "ChangePopWealthTypeMultis") ChangePopWealthTypeMultis(ParamValues);
  else checkNoEntry();
}
void ACustomerAIManager::ChangeActionWeights(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    ECustomerAction Action = ECustomerAction::None;
    for (auto A : TEnumRange<ECustomerAction>()) {
      if (UEnum::GetDisplayValueAsText(A).ToString() == ParamPair.Key) {
        Action = A;
        break;
      }
    }
    check(Action != ECustomerAction::None);

    BehaviorParams.ActionWeights[Action] += ParamPair.Value;
  }
}
void ACustomerAIManager::ChangePopTypeMultis(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    EPopType PopType = EPopType::Common;
    for (auto P : TEnumRange<EPopType>()) {
      if (UEnum::GetDisplayValueAsText(P).ToString() == ParamPair.Key) {
        PopType = P;
        break;
      }
    }

    BehaviorParams.PopTypeMultis[PopType] += ParamPair.Value;
  }
}
void ACustomerAIManager::ChangePopWealthTypeMultis(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    EPopWealthType WealthType = EPopWealthType::Poorer;
    for (auto W : TEnumRange<EPopWealthType>()) {
      if (UEnum::GetDisplayValueAsText(W).ToString() == ParamPair.Key) {
        WealthType = W;
        break;
      }
    }

    BehaviorParams.PopWealthTypeMultis[WealthType] += ParamPair.Value;
  }
}