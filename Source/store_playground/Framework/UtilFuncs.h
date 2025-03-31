// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// * Utility functions.
// Mostly templates for generic functions.

template <typename T>
T GetWeightedRandomItem(const TArray<T>& Items, std::function<float(const T&)> WeightFunc) {
  float TotalWeight = 0.0f;
  for (const T& Item : Items) TotalWeight += WeightFunc(Item);

  float RandomItem = FMath::FRandRange(0.0f, TotalWeight);
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