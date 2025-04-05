// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECustomerAction : uint8 {
  None UMETA(DisplayName = "NONE"),
  PickItem UMETA(DisplayName = "PickItem"),
  StockCheck UMETA(DisplayName = "StockCheck"),
  SellItem UMETA(DisplayName = "SellItem"),
  Leave UMETA(DisplayName = "Leave"),
};
ENUM_RANGE_BY_COUNT(ECustomerAction, 5);