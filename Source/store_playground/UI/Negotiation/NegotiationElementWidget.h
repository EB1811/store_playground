// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NegotiationElementWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationElementWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UButton* OfferButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* AcceptButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* RejectButton;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemName;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* BoughtAtPrice;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* MarketPrice;

  UPROPERTY(meta = (BindWidget))
  class UImage* ItemIcon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NegotiationStateText;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* OfferedPrice;
  UPROPERTY(meta = (BindWidget))
  class USlider* PlayerOfferedPrice;
};