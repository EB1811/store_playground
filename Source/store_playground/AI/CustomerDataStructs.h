// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimInstance.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "CustomerDataStructs.generated.h"

USTRUCT()
struct FCustomerAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> IdleSprites;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> WalkSprites;
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

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;
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

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;
};

// Dynamic subtypes for stock check requests.
USTRUCT()
struct FWantedItemType {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName WantedItemTypeID;

  UPROPERTY(EditAnywhere)
  FText WantedItemTypeName;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;
};
USTRUCT()
struct FWantedItemTypeRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName WantedItemTypeID;

  UPROPERTY(EditAnywhere)
  FText WantedItemTypeName;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;
};