// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// * Utility functions and structs.
// Mostly templates for generic functions.

template <typename T>
T GetWeightedRandomItem(const TArray<T>& Items, std::function<int32(const T&)> WeightFunc) {
  int32 TotalWeight = 0;
  for (const T& Item : Items) TotalWeight += WeightFunc(Item);

  int32 RandomItem = FMath::RandRange(0, TotalWeight);
  for (const T& Item : Items) {
    if (RandomItem < WeightFunc(Item)) return Item;
    RandomItem -= WeightFunc(Item);
  }

  return Items[0];
}

// Save as a variable first.
// Copies the array, so don't use it on large arrays.
template <typename T>
TArray<T*> GetAllActorsOf(UWorld* World, TSubclassOf<class AActor> ActorClass) {
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActors);

  TArray<T*> FoundActorsOfT;
  FoundActorsOfT.Reserve(FoundActors.Num());
  for (AActor* Actor : FoundActors) {
    T* CastedActor = Cast<T>(Actor);
    check(CastedActor);

    FoundActorsOfT.Add(CastedActor);
  }

  return FoundActorsOfT;
}

template <typename T>
TArray<FName> FormIdList(const TArray<T>& Items, std::function<FName(const T&)> IdFunc) {
  TArray<FName> Ids;
  Ids.Reserve(Items.Num());
  for (const auto& Item : Items) Ids.Add(IdFunc(Item));
  return Ids;
}

// * Dynamically change a parameter value in a struct using Unreal's reflection system.
inline void SetStructPropertyValue(FStructProperty* StructProp,
                                   void* StructPtr,
                                   const FName& PropName,
                                   float PropValue) {
  auto Prop = StructProp->Struct->FindPropertyByName(PropName);
  check(Prop);

  if (Prop->IsA<FFloatProperty>()) {
    float* PropValuePtr = Prop->ContainerPtrToValuePtr<float>(StructPtr);
    *PropValuePtr = FMath::Max(*PropValuePtr, 1.0f) * PropValue;
  } else if (Prop->IsA<FIntProperty>()) {
    int32* PropValuePtr = Prop->ContainerPtrToValuePtr<int32>(StructPtr);
    *PropValuePtr = static_cast<int32>(FMath::Max(*PropValuePtr, 1) * PropValue);
  }
}

inline FGameplayTagContainer StringTagsToContainer(const TArray<FName>& Tags) {
  TArray<FGameplayTag> TagsArray;
  for (const auto& TagString : Tags) {
    auto Tag = FGameplayTag::RequestGameplayTag(TagString);
    check(Tag.IsValid());
    TagsArray.Add(Tag);
  }
  return FGameplayTagContainer::CreateFromArray(TagsArray);
}