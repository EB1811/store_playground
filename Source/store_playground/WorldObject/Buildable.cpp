#include "Buildable.h"
#include "Components/StaticMeshComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "PaperFlipbookComponent.h"
#include "NavModifierComponent.h"

ABuildable::ABuildable() {
  PrimaryActorTick.bCanEverTick = true;

  if (!BuildableId.IsValid()) BuildableId = FGuid::NewGuid();

  SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
  SetRootComponent(SceneRoot);

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(SceneRoot);
  Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
  Sprite->SetupAttachment(SceneRoot);

  NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  StockInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("StockInventory"));
  StockDisplay = CreateDefaultSubobject<UStockDisplayComponent>(TEXT("StockDisplayComponent"));
}

void ABuildable::BeginPlay() {
  Super::BeginPlay();

  check(BuildableId.IsValid());

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

  NavModifier->SetAreaClass(UNavArea_Null::StaticClass());

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

  NavModifier->SetAreaClass(nullptr);

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

void LoadBuildableSaveState(ABuildable* Buildable) {
  switch (Buildable->BuildableType) {
    case EBuildableType::StockDisplay: Buildable->SetToStockDisplay(); break;
    case EBuildableType::Decoration: Buildable->SetToDecoration(); break;
    default: Buildable->SetToNone();
  }
}