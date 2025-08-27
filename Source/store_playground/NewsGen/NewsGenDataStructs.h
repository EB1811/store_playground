// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NewsGenDataStructs.generated.h"

UENUM()
enum class EArticleSize : uint8 {
  Small UMETA(DisplayName = "Small"),
  Medium UMETA(DisplayName = "Medium"),
  Large UMETA(DisplayName = "Large"),
};

USTRUCT()
struct FArticleTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FText Title;
  UPROPERTY(EditAnywhere, SaveGame, meta = (MultiLine = true))
  FText Body;
};
USTRUCT()
struct FArticleAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  class UMaterialInstance* Picture;
};

USTRUCT()
struct FArticle {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ArticleID;

  UPROPERTY(EditAnywhere, SaveGame)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere, SaveGame)
  int32 AppearWeight;
  UPROPERTY(EditAnywhere, SaveGame)
  bool bIsRepeatable;
  UPROPERTY(EditAnywhere, SaveGame)
  bool bIsSpecial;  // * Econ event articles, etc.
  UPROPERTY(EditAnywhere)
  bool bIsTriggered;

  UPROPERTY(EditAnywhere, SaveGame)
  EArticleSize Size;
  UPROPERTY(EditAnywhere, SaveGame)
  FArticleTextData TextData;
  UPROPERTY(EditAnywhere, SaveGame)
  FArticleAssetData AssetData;

  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer Tags;  // * Tags for systems to filter further.
};
USTRUCT()
struct FArticleRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ArticleID;

  UPROPERTY(EditAnywhere)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere)
  int32 AppearWeight;  // * 0 = never appears
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;
  UPROPERTY(EditAnywhere)
  bool bIsSpecial;  // * Econ event articles, etc.
  UPROPERTY(EditAnywhere)
  bool bIsTriggered;

  UPROPERTY(EditAnywhere)
  EArticleSize Size;
  UPROPERTY(EditAnywhere)
  FArticleTextData TextData;
  UPROPERTY(EditAnywhere)
  FArticleAssetData AssetData;

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;  // * Tags for systems to filter further.
};