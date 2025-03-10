// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "CustomerDataStructs.generated.h"

USTRUCT()
struct FCustomerAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  UStaticMesh* Mesh;
  // UPROPERTY(EditAnywhere)
  // UMaterialInterface* Material;
};

USTRUCT()
struct FGenericCustomerData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName CustomerID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;

  UPROPERTY(EditAnywhere)
  FText CustomerName;

  UPROPERTY(EditAnywhere)
  ECustomerAttitude InitAttitude;

  UPROPERTY(EditAnywhere)
  FCustomerAssetData AssetData;
};

USTRUCT()
struct FCustomerDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName CustomerID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;

  UPROPERTY(EditAnywhere)
  FText CustomerName;
  UPROPERTY(EditAnywhere)
  FText LinkedPopName;

  UPROPERTY(EditAnywhere)
  ECustomerAttitude InitAttitude;

  UPROPERTY(EditAnywhere)
  FCustomerAssetData AssetData;
};