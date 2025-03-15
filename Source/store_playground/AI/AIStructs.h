// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECustomerAction : uint8 {
  None UMETA(DisplayName = "NONE"),
  PickItem UMETA(DisplayName = "Pick Item"),
  StockCheck UMETA(DisplayName = "Stock Check"),
  SellItem UMETA(DisplayName = "Sell Item"),
  Leave UMETA(DisplayName = "Leave"),
};