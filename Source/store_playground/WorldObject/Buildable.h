// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildable.generated.h"

UENUM()
enum class EBuildableType : uint8 {
  None UMETA(DisplayName = "None"),
  StockDisplay UMETA(DisplayName = "StockDisplay"),
  Decoration UMETA(DisplayName = "Decoration"),
};
ENUM_RANGE_BY_COUNT(EBuildableType, 3);

UCLASS()
class STORE_PLAYGROUND_API ABuildable : public AActor {
  GENERATED_BODY()

public:
  ABuildable();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Buildable")
  FGuid BuildableId;

  // ? Buildable component?
  UPROPERTY(EditAnywhere, Category = "Buildable")
  TMap<EBuildableType, UStaticMesh*> MeshesMap;
  UPROPERTY(EditAnywhere, Category = "Buildable")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Buildable")
  TMap<EBuildableType, bool> IsBuildableMap;
  UPROPERTY(EditAnywhere, Category = "Buildable")
  TMap<EBuildableType, float> BuildingPricesMap;
  UPROPERTY(EditAnywhere, Category = "Buildable")
  EBuildableType BuildableType;

  UPROPERTY(EditAnywhere, Category = "Buildable")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "Buildable")
  class UInventoryComponent* StockInventory;

  UPROPERTY(EditAnywhere, Category = "Buildable")
  class UStockDisplayComponent* StockDisplay;

  void SetToStockDisplay();
  void SetToDecoration();
  void SetToNone();

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

USTRUCT()
struct FBuildableSaveState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid BuildableId;

  UPROPERTY(EditAnywhere)
  EBuildableType BuildableType;
  UPROPERTY(EditAnywhere)
  TArray<TObjectPtr<class UItemBase>> ItemsArray;  // Temp.
};
FBuildableSaveState SaveBuildableSaveState(ABuildable* Buildable);
void LoadBuildableSaveState(ABuildable* Buildable, FBuildableSaveState SaveState);