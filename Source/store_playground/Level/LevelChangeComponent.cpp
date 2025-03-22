// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelChangeComponent.h"

ULevelChangeComponent::ULevelChangeComponent() { PrimaryComponentTick.bCanEverTick = false; }

void ULevelChangeComponent::BeginPlay() { Super::BeginPlay(); }