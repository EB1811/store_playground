#include "LootboxMinigame.h"
#include "CommonActionWidget.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Pickup/PickupComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/WorldObject/Level/PickupActor.h"
#include "store_playground/Store/Store.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void ULootboxMinigame::NativeOnInitialized() {
  Super::NativeOnInitialized();

  OpenLootboxButton->OnClicked.AddDynamic(this, &ULootboxMinigame::Open);
  BackButton->OnClicked.AddDynamic(this, &ULootboxMinigame::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void ULootboxMinigame::Open() {
  check(Market && PlayerInventory);
  if (Store->Money < Price) {
    ErrorText->SetText(FText::FromString("Not enough money!"));
    ErrorText->SetVisibility(ESlateVisibility::Visible);
    return;
  }
  ErrorText->SetVisibility(ESlateVisibility::Hidden);

  Store->MoneySpent(Price);

  int32 QuantityMin =
      FMath::Clamp((Price / LootboxMinigameParams.BaseLootboxPriceMax) * (LootboxMinigameParams.MaxItems / 2), 1,
                   LootboxMinigameParams.MaxItems);
  int32 RandomQuantity = FMath::RandRange(QuantityMin, LootboxMinigameParams.MaxItems);
  TArray<UItemBase*> Items = Market->GetNewRandomItems(RandomQuantity);

  AActor* MiniGameOwner = MiniGameC->GetOwner();
  FActorSpawnParameters NpcStoreSpawnParams;
  NpcStoreSpawnParams.Owner = MiniGameOwner;
  NpcStoreSpawnParams.bNoFail = true;
  NpcStoreSpawnParams.SpawnCollisionHandlingOverride =
      ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  NpcStoreSpawnParams.OverrideLevel = MiniGameOwner->GetLevel();
  for (auto& Item : Items) {
    float SpaceAround = 100.0f;
    FVector SpawnLocation =
        MiniGameOwner->GetActorLocation() + FVector(FMath::RandRange(-SpaceAround + -75.0f, 0.0f),
                                                    FMath::RandRange(-SpaceAround + -75.0f, SpaceAround + 75.0f),
                                                    -MiniGameOwner->GetActorLocation().Z);
    APickupActor* Pickup = GetWorld()->SpawnActor<APickupActor>(PickupActorClass, SpawnLocation, FRotator::ZeroRotator,
                                                                NpcStoreSpawnParams);
    Pickup->PickupComponent->InitPickup(EPickupGoodType::Item, 0, Item->ItemID);
    Pickup->InteractionComponent->InteractionType = EInteractionType::Pickup;
  }

  UGameplayStatics::PlaySound2D(GetWorld(), OpenBoxSound);

  Price = 0;
  Back();
}

void ULootboxMinigame::Back() { CloseWidgetFunc(this); }

void ULootboxMinigame::InitUI(FInUIInputActions InUIInputActions,
                              const class AMarket* _Market,
                              class UMiniGameComponent* _MiniGameC,
                              class AStore* _Store,
                              class UInventoryComponent* _PlayerInv,
                              TMap<FName, float> _PersistentData,
                              std::function<void(TMap<FName, float>)> _UpdatePersistentDataFunc,
                              std::function<void(class UUserWidget*)> _CloseWidgetFunc) {
  check(_MiniGameC && _Market && _Store && _PlayerInv && _UpdatePersistentDataFunc && _CloseWidgetFunc);

  Market = _Market;
  MiniGameC = _MiniGameC;
  Store = _Store;
  PlayerInventory = _PlayerInv;
  PersistentData = _PersistentData;
  UpdatePersistentDataFunc = _UpdatePersistentDataFunc;
  CloseWidgetFunc = _CloseWidgetFunc;

  if (Price <= 0) {
    Price = FMath::FRandRange(LootboxMinigameParams.BaseLootboxPriceMin, LootboxMinigameParams.BaseLootboxPriceMax);
    PriceText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Price)));
  }

  if (Price > Store->Money) OpenLootboxButton->SetIsEnabled(false);
  else OpenLootboxButton->SetIsEnabled(true);

  PriceText->SetVisibility(ESlateVisibility::Visible);
  OpenLootboxButton->SetVisibility(ESlateVisibility::Visible);
  ErrorText->SetVisibility(ESlateVisibility::Hidden);

  OpenActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
}

void ULootboxMinigame::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (OpenLootboxButton->GetIsEnabled()) Open();
  };
  UIActionable.RetractUI = [this]() { Back(); };
  UIActionable.QuitUI = [this]() { Back(); };
}

void ULootboxMinigame::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}