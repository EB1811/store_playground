#include "Buildable.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"

ABuildable::ABuildable() {
  PrimaryActorTick.bCanEverTick = true;

  if (!BuildableId.IsValid()) BuildableId = FGuid::NewGuid();

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  StockInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("StockInventory"));
  StockDisplay = CreateDefaultSubobject<UStockDisplayComponent>(TEXT("StockDisplayComponent"));
}

void ABuildable::BeginPlay() {
  Super::BeginPlay();

  for (auto Type : TEnumRange<EBuildableType>()) {
    check(MeshesMap.Contains(Type));
    check(IsBuildableMap.Contains(Type));
    check(BuildingPricesMap.Contains(Type));
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

  Mesh->SetVisibility(true);
  Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

  InteractionComponent->InteractionType = EInteractionType::StockDisplay;
}

void ABuildable::SetToDecoration() {
  if (!IsBuildableMap[EBuildableType::Decoration]) return;

  BuildableType = EBuildableType::Decoration;
  Mesh->SetStaticMesh(MeshesMap[BuildableType]);

  Mesh->SetVisibility(true);
  Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

  InteractionComponent->InteractionType = EInteractionType::Decoration;
}

void ABuildable::SetToNone() {
  BuildableType = EBuildableType::None;
  Mesh->SetStaticMesh(MeshesMap[BuildableType]);

  Mesh->SetVisibility(false);
  Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

FBuildableSaveState SaveBuildableSaveState(ABuildable* Buildable) {
  return {Buildable->BuildableId, Buildable->BuildableType, {}};
}

void LoadBuildableSaveState(ABuildable* Buildable, FBuildableSaveState SaveState) {
  UE_LOG(LogTemp, Warning, TEXT("Buildable setting to %s"), *UEnum::GetValueAsString(Buildable->BuildableType));
  switch (Buildable->BuildableType) {
    case EBuildableType::StockDisplay: Buildable->SetToStockDisplay(); break;
    case EBuildableType::Decoration: Buildable->SetToDecoration(); break;
    default: Buildable->SetToNone();
  }
}