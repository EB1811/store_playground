// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestComponent.h"

UQuestComponent::UQuestComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UQuestComponent::BeginPlay() { Super::BeginPlay(); }