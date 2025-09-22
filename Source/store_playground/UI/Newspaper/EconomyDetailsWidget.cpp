#include "EconomyDetailsWidget.h"
#include "Components/WrapBoxSlot.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/UI/Newspaper/PopDetailsWidget.h"
#include "store_playground/UI/Newspaper/PopTypeWrapBox.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "PaperSprite.h"
#include "Blueprint/WidgetTree.h"

inline auto GetWealthText(TArray<FPopEconData> PopEconDataArray, const FPopEconData& GivenPop) -> FString {
  float TotalGoodsBoughtPerCapita = 0.0f;
  for (auto& Pop : PopEconDataArray) TotalGoodsBoughtPerCapita += Pop.GoodsBoughtPerCapita;

  float GivenPopGoodsBoughtPercentage = GivenPop.GoodsBoughtPerCapita / TotalGoodsBoughtPerCapita * 100.0f;
  UE_LOG(LogTemp, Log, TEXT("GivenPopGoodsBoughtPercentage for %s: %.2f%%"), *GivenPop.PopID.ToString(),
         GivenPopGoodsBoughtPercentage);

  if (GivenPopGoodsBoughtPercentage <= 0.0f) return "None";
  if (GivenPopGoodsBoughtPercentage < 0.5f) return "Very Low";
  if (GivenPopGoodsBoughtPercentage < 1.5f) return "Low";
  if (GivenPopGoodsBoughtPercentage < 3.0f) return "Middle";
  if (GivenPopGoodsBoughtPercentage < 6.0f) return "High";
  if (GivenPopGoodsBoughtPercentage < 10.0f) return "Very High";
  return "Immense";
}
inline auto GetItemSpendText(TMap<EItemWealthType, float> ItemSpendPercent) -> FString {
  TArray<FString> SpendStrings;
  for (const auto& Spend : ItemSpendPercent)
    if (Spend.Value >= 40.0f) SpendStrings.Add(UEnum::GetDisplayValueAsText(Spend.Key).ToString());
  if (SpendStrings.Num() <= 0) return "All";

  return FString::Join(SpendStrings, TEXT(" & "));
}

inline auto IsWealthTrendUp(TArray<float> GoodsBoughtPerCapitaHistory) -> bool {
  int32 ConsideredHistoryCount = 2;
  if (GoodsBoughtPerCapitaHistory.Num() < ConsideredHistoryCount) return false;

  float SignificantChange = 1.0f;
  return GoodsBoughtPerCapitaHistory.Last() >
         GoodsBoughtPerCapitaHistory[GoodsBoughtPerCapitaHistory.Num() - ConsideredHistoryCount] + SignificantChange;
}
inline auto IsWealthTrendDown(TArray<float> GoodsBoughtPerCapitaHistory) -> bool {
  int32 ConsideredHistoryCount = 2;
  if (GoodsBoughtPerCapitaHistory.Num() < ConsideredHistoryCount) return false;

  float SignificantChange = 1.0f;
  return GoodsBoughtPerCapitaHistory.Last() <
         GoodsBoughtPerCapitaHistory[GoodsBoughtPerCapitaHistory.Num() - ConsideredHistoryCount] - SignificantChange;
}
inline auto IsPopulationTrendUp(TArray<float> PopulationHistory) -> bool {
  int32 ConsideredHistoryCount = 5;
  if (PopulationHistory.Num() < ConsideredHistoryCount) return false;

  float SignificantChange = 0.9f;
  return PopulationHistory.Last() >
         PopulationHistory[PopulationHistory.Num() - ConsideredHistoryCount] + SignificantChange;
}
inline auto IsPopulationTrendDown(TArray<float> PopulationHistory) -> bool {
  int32 ConsideredHistoryCount = 5;
  if (PopulationHistory.Num() < ConsideredHistoryCount) return false;

  float SignificantChange = 0.9f;
  return PopulationHistory.Last() <
         PopulationHistory[PopulationHistory.Num() - ConsideredHistoryCount] - SignificantChange;
}

void UEconomyDetailsWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UEconomyDetailsWidget::RefreshUI() {
  // * First group pops by type and sort by wealth type.
  TMap<EPopType, TArray<TTuple<FCustomerPop, FPopEconData>>> PopTypeToDataMap;
  for (auto PopType : TEnumRange<EPopType>())
    PopTypeToDataMap.Add(PopType, TArray<TTuple<FCustomerPop, FPopEconData>>());

  for (int32 i = 0; i < MarketEconomy->CustomerPops.Num(); i++) {
    const FCustomerPop Pop = MarketEconomy->CustomerPops[i];
    const FPopEconData PopEconData = MarketEconomy->PopEconDataArray[i];

    PopTypeToDataMap[Pop.PopType].Add({Pop, PopEconData});
  }
  for (auto PopType : TEnumRange<EPopType>()) {
    TArray<TTuple<FCustomerPop, FPopEconData>>& PopDataArray = PopTypeToDataMap[PopType];
    PopDataArray.Sort([](const TTuple<FCustomerPop, FPopEconData>& A, const TTuple<FCustomerPop, FPopEconData>& B) {
      if (A.Get<0>().WealthType == B.Get<0>().WealthType) return A.Get<0>().PopName.CompareTo(B.Get<0>().PopName) < 0;

      return static_cast<int32>(A.Get<0>().WealthType) < static_cast<int32>(B.Get<0>().WealthType);
    });
  }

  PopDetailsBox->ClearChildren();
  for (auto PopType : TEnumRange<EPopType>()) {
    const TArray<TTuple<FCustomerPop, FPopEconData>>& PopDataArray = PopTypeToDataMap[PopType];
    if (PopDataArray.Num() <= 0) continue;

    TTuple<UPopDetailsWidget*, UWrapBoxSlot*> FirstPopDetailsWidget = {nullptr, nullptr};
    for (const TTuple<FCustomerPop, FPopEconData>& PopData : PopDataArray) {
      const FCustomerPop& Pop = PopData.Get<0>();
      const FPopEconData& PopEconData = PopData.Get<1>();

      UPopDetailsWidget* PopDetailsWidget = CreateWidget<UPopDetailsWidget>(this, PopDetailsWidgetClass);
      check(PopDetailsWidget);

      // ! Workaround for two different asset sizes.
      if (Pop.AssetData.Sprite->GetSlateAtlasData().GetSourceDimensions().Y >= 48) {
        PopDetailsWidget->Page2Icon->SetBrushFromAtlasInterface(Pop.AssetData.Sprite);
        PopDetailsWidget->Page2Icon->SetVisibility(ESlateVisibility::Visible);
        PopDetailsWidget->Icon->SetVisibility(ESlateVisibility::Collapsed);
      } else {
        PopDetailsWidget->Icon->SetBrushFromAtlasInterface(Pop.AssetData.Sprite);
        PopDetailsWidget->Icon->SetVisibility(ESlateVisibility::Visible);
        PopDetailsWidget->Page2Icon->SetVisibility(ESlateVisibility::Collapsed);
      }

      PopDetailsWidget->Name->SetText(Pop.PopName);
      PopDetailsWidget->WealthText->SetText(
          FText::FromString("Wealth: " + GetWealthText(MarketEconomy->PopEconDataArray, PopEconData)));
      // float PopulationPercent = float(PopEconData.Population) / float(MarketEconomy->TotalPopulation) * 100.0f;
      // PopDetailsWidget->PopulationText->SetText(FText::FromString(
      //     FString::Printf(PopulationPercent < 1.0f ? TEXT("Population: %.1f") : TEXT("Population: %.0f"),
      //                     PopulationPercent) +
      //     "%"));
      PopDetailsWidget->PopulationText->SetText(
          FText::FromString(FString::Printf(TEXT("Population: %d"), PopEconData.Population)));
      PopDetailsWidget->ItemSpendText->SetText(
          FText::FromString("Spends On: " + GetItemSpendText(Pop.ItemSpendPercent)));
      FString EconTypesText = Pop.ItemEconTypes.Num() < 4
                                  ? FString::JoinBy(Pop.ItemEconTypes, TEXT(" & "),
                                                    [](const EItemEconType& EconType) {
                                                      return UEnum::GetDisplayValueAsText(EconType).ToString();
                                                    })
                                  : "All";
      PopDetailsWidget->ItemEconTypesText->SetText(FText::FromString("Goods Type: " + EconTypesText));

      if (StatisticsGen->PopStatisticsMap.Num() > 0) {
        auto GoodsBoughtPerCapitaHistory =
            StatisticsGen->PopStatisticsMap[PopEconData.PopID].GoodsBoughtPerCapitaHistory;
        if (IsWealthTrendUp(GoodsBoughtPerCapitaHistory)) {
          PopDetailsWidget->WealthTrendUpIcon->SetVisibility(ESlateVisibility::Visible);
          PopDetailsWidget->WealthTrendDownIcon->SetVisibility(ESlateVisibility::Collapsed);
        } else if (IsWealthTrendDown(GoodsBoughtPerCapitaHistory)) {
          PopDetailsWidget->WealthTrendUpIcon->SetVisibility(ESlateVisibility::Collapsed);
          PopDetailsWidget->WealthTrendDownIcon->SetVisibility(ESlateVisibility::Visible);
        } else {
          PopDetailsWidget->WealthTrendUpIcon->SetVisibility(ESlateVisibility::Collapsed);
          PopDetailsWidget->WealthTrendDownIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
        auto PopulationHistory = StatisticsGen->PopStatisticsMap[PopEconData.PopID].PopulationHistory;
        if (IsPopulationTrendUp(PopulationHistory)) {
          PopDetailsWidget->PopulationTrendUpIcon->SetVisibility(ESlateVisibility::Visible);
          PopDetailsWidget->PopulationTrendDownIcon->SetVisibility(ESlateVisibility::Collapsed);
        } else if (IsPopulationTrendDown(PopulationHistory)) {
          PopDetailsWidget->PopulationTrendUpIcon->SetVisibility(ESlateVisibility::Collapsed);
          PopDetailsWidget->PopulationTrendDownIcon->SetVisibility(ESlateVisibility::Visible);
        } else {
          PopDetailsWidget->PopulationTrendUpIcon->SetVisibility(ESlateVisibility::Collapsed);
          PopDetailsWidget->PopulationTrendDownIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
      }

      if (!FirstPopDetailsWidget.Key)
        FirstPopDetailsWidget = {PopDetailsWidget, PopDetailsBox->AddChildToWrapBox(PopDetailsWidget)};
      else PopDetailsBox->AddChildToWrapBox(PopDetailsWidget);
    }

    FirstPopDetailsWidget.Key->BgBorder->SetBrushFromMaterial(PopTypeMaterialMap[PopType]);
    FirstPopDetailsWidget.Value->SetNewLine(true);  // Force the first pop details widget to start a new line.
  }
}

void UEconomyDetailsWidget::InitUI(const class AMarketEconomy* _MarketEconomy,
                                   const class AStatisticsGen* _StatisticsGen) {
  check(_MarketEconomy && _StatisticsGen);

  MarketEconomy = _MarketEconomy;
  StatisticsGen = _StatisticsGen;
}