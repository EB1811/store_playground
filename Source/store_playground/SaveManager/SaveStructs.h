// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/SlateEnums.h"
#include "SaveStructs.generated.h"

USTRUCT()
struct FSystemSaveState {
  GENERATED_BODY()

  UPROPERTY()
  FName Name;

  UPROPERTY()
  TArray<uint8> ByteData;
};

USTRUCT()
struct FLevelSaveState {
  GENERATED_BODY()

  UPROPERTY()
  FName Name;

  UPROPERTY()
  TArray<FGuid> ActorIds;
  UPROPERTY()
  TArray<FGuid> ActorComponentIds;
  UPROPERTY()
  TArray<FGuid> ComponentObjectIds;
};

USTRUCT()
struct FActorSavaState {
  GENERATED_BODY()

  UPROPERTY()
  FGuid Id;

  UPROPERTY()
  TArray<uint8> ByteData;

  UPROPERTY()
  TMap<FName, FGuid> ActorComponentsMap;  // * Component name -> component ID.
};

USTRUCT()
struct FComponentSaveState {
  GENERATED_BODY()

  UPROPERTY()
  FGuid Id;

  UPROPERTY()
  TArray<uint8> ByteData;

  UPROPERTY()
  TArray<FGuid> ComponentObjects;
};

USTRUCT()
struct FObjectSaveState {
  GENERATED_BODY()

  UPROPERTY()
  FGuid Id;

  UPROPERTY()
  TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSavaState {
  GENERATED_BODY()

  UPROPERTY()
  TMap<FName, FGuid> ActorComponentsMap;  // * Component name -> component ID.
};

// Helper struct.
USTRUCT()
struct FLevelsSaveData {
  GENERATED_BODY()

  UPROPERTY()
  TMap<FName, FLevelSaveState> LevelSaveMap;
  UPROPERTY()
  TMap<FGuid, FActorSavaState> ActorSaveMap;
  UPROPERTY()
  TMap<FGuid, FComponentSaveState> ComponentSaveMap;
  UPROPERTY()
  TArray<FObjectSaveState> ObjectSaveStates;
};

USTRUCT()
struct FSaveSlotData {
  GENERATED_BODY()

  UPROPERTY()
  bool bIsPopulated;  // * Save game or empty slot.

  UPROPERTY()
  FString SlotName;
  UPROPERTY()
  FDateTime LastModified;

  // * Info to display to the user.
  UPROPERTY()
  int32 CurrentDay;
  UPROPERTY()
  float StoreMoney;
};