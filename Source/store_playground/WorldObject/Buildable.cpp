#include "Buildable.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"

ABuildable::ABuildable() {
  PrimaryActorTick.bCanEverTick = true;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  StockInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("StockInventory"));
  StockDisplay = CreateDefaultSubobject<UStockDisplayComponent>(TEXT("StockDisplayComponent"));
}

void ABuildable::BeginPlay() {
  Super::BeginPlay();

  for (auto BType : TEnumRange<EBuildableType>()) {
    check(MeshesMap.Contains(BType));
    check(IsBuildableMap.Contains(BType));
    check(BuildingPricesMap.Contains(BType));
  }

  switch (BuildableType) {
    case EBuildableType::StockDisplay: SetToStockDisplay(); break;
    case EBuildableType::Decoration: SetToDecoration(); break;
    default: SetToNone();
  }
}

void ABuildable::SetToStockDisplay() {
  if (!IsBuildableMap[EBuildableType::StockDisplay]) return;

  BuildableType = EBuildableType::StockDisplay;
  Mesh->SetStaticMesh(MeshesMap[BuildableType]);

  InteractionComponent->InteractionType = EInteractionType::StockDisplay;
}

void ABuildable::SetToDecoration() {
  if (!IsBuildableMap[EBuildableType::Decoration]) return;

  BuildableType = EBuildableType::Decoration;
  Mesh->SetStaticMesh(MeshesMap[BuildableType]);

  InteractionComponent->InteractionType = EInteractionType::Decoration;
}

void ABuildable::SetToNone() {
  BuildableType = EBuildableType::None;
  Mesh->SetStaticMesh(MeshesMap[BuildableType]);

  InteractionComponent->InteractionType = EInteractionType::Buildable;
}

#if WITH_EDITOR
void ABuildable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
  Super::PostEditChangeProperty(PropertyChangedEvent);

  const FName ChangedPropertyName = PropertyChangedEvent.GetPropertyName();
  if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ABuildable, BuildableType)) {
    switch (BuildableType) {
      case EBuildableType::StockDisplay: SetToStockDisplay(); break;
      case EBuildableType::Decoration: SetToDecoration(); break;
      default: SetToNone();
    }
  }
}
#endif