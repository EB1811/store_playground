#include "EconomyDetailsWidget.h"
#include "Components/WrapBoxSlot.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/UI/Newspaper/PopDetailsWidget.h"
#include "store_playground/UI/Newspaper/PopTypeWrapBox.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/UI/Graph/StoreStatsGraphWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "PaperSprite.h"
#include "Blueprint/WidgetTree.h"

inline auto GetGensString(const class AMarketEconomy* MarketEconomy, const FPopEconData& GivenPop) -> FString {
  float GenPercent = ((float(GivenPop.MGen) * float(GivenPop.Population) * MarketEconomy->BehaviorParams.MGenMulti) /
                      MarketEconomy->TotalWealth) *
                     100.0f;

  return FString::Printf(GenPercent < 1.0f ? TEXT("Produces: %.1f") : TEXT("Produces: %.0f"), GenPercent) + "%";
}
inline auto GetWealthString(TArray<FPopEconData> PopEconDataArray, const FPopEconData& GivenPop) -> FString {
  float TotalGoodsBoughtPerCapita = 0.0f;
  for (auto& Pop : PopEconDataArray) TotalGoodsBoughtPerCapita += Pop.GoodsBoughtPerCapita;

  float GivenPopGoodsBoughtPercentage = GivenPop.GoodsBoughtPerCapita / TotalGoodsBoughtPerCapita * 100.0f;
  // UE_LOG(LogTemp, Log, TEXT("GivenPopGoodsBoughtPercentage for %s: %.2f%%"), *GivenPop.PopID.ToString(),
  //        GivenPopGoodsBoughtPercentage);

  return FString::Printf(GivenPopGoodsBoughtPercentage < 1.0f ? TEXT("Wealth: %.1f") : TEXT("Wealth: %.0f"),
                         GivenPopGoodsBoughtPercentage) +
         "%";

  // if (GivenPopGoodsBoughtPercentage <= 0.0f) return "None";
  // if (GivenPopGoodsBoughtPercentage < 0.5f) return "Very Low";
  // if (GivenPopGoodsBoughtPercentage < 1.5f) return "Low";
  // if (GivenPopGoodsBoughtPercentage < 3.0f) return "Middle";
  // if (GivenPopGoodsBoughtPercentage < 6.0f) return "High";
  // if (GivenPopGoodsBoughtPercentage < 10.0f) return "Very High";
  // return "Immense";
}
inline auto GetItemSpendString(TMap<EItemWealthType, float> ItemSpendPercent) -> FString {
  TArray<FString> SpendStrings;
  for (const auto& Spend : ItemSpendPercent)
    if (Spend.Value >= 40.0f) SpendStrings.Add(GetItemWealthTypeText(Spend.Key).ToString());
  if (SpendStrings.Num() <= 0) return "All";

  return FString::Join(SpendStrings, TEXT(" & "));
}

inline auto IsWealthTrendUp(TArray<float> GoodsBoughtPerCapitaHistory) -> bool {
  int32 ConsideredHistoryCount = FMath::Min(10, GoodsBoughtPerCapitaHistory.Num());
  float SignificantChange = 1.0f;
  return GoodsBoughtPerCapitaHistory.Last() >
         GoodsBoughtPerCapitaHistory[GoodsBoughtPerCapitaHistory.Num() - ConsideredHistoryCount] + SignificantChange;
}
inline auto IsWealthTrendDown(TArray<float> GoodsBoughtPerCapitaHistory) -> bool {
  int32 ConsideredHistoryCount = FMath::Min(10, GoodsBoughtPerCapitaHistory.Num());
  float SignificantChange = 1.0f;
  return GoodsBoughtPerCapitaHistory.Last() <
         GoodsBoughtPerCapitaHistory[GoodsBoughtPerCapitaHistory.Num() - ConsideredHistoryCount] - SignificantChange;
}
inline auto IsPopulationTrendUp(TArray<float> PopulationHistory) -> bool {
  int32 ConsideredHistoryCount = FMath::Min(10, PopulationHistory.Num());
  float SignificantChange = 0.9f;
  return PopulationHistory.Last() >
         PopulationHistory[PopulationHistory.Num() - ConsideredHistoryCount] + SignificantChange;
}
inline auto IsPopulationTrendDown(TArray<float> PopulationHistory) -> bool {
  int32 ConsideredHistoryCount = FMath::Min(10, PopulationHistory.Num());
  float SignificantChange = 0.9f;
  return PopulationHistory.Last() <
         PopulationHistory[PopulationHistory.Num() - ConsideredHistoryCount] - SignificantChange;
}

void UEconomyDetailsWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UEconomyDetailsWidget::RefreshUI() {
  if (StatisticsGen->EconomyStatistics.TotalWealthHistory.Num() > 0 &&
      StatisticsGen->EconomyStatistics.TotalInflationHistory.Num() > 0) {
    float TotalWealth = StatisticsGen->EconomyStatistics.TotalWealthHistory.Last();
    TotalWealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), TotalWealth)));
    float TotalBought = StatisticsGen->EconomyStatistics.TotalBoughtHistory.Last();
    TotalBoughtText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), TotalBought)));
    float TotalInflation = StatisticsGen->EconomyStatistics.TotalInflationHistory.Last();
    TotalInflationText->SetText(FText::FromString(FString::Printf(TEXT("%.2f%%"), TotalInflation)));

    TotalWealthGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalWealthHistory);
    TotalWealthGraphWidget->CreateStoreStatsGraph();
    TotalBoughtGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalBoughtHistory);
    TotalBoughtGraphWidget->CreateStoreStatsGraph();
    TotalInflationGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalInflationHistory);
    TotalInflationGraphWidget->CreateStoreStatsGraph();
  }

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

      PopDetailsWidget->PopID = Pop.ID;

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
      PopDetailsWidget->GensText->SetText(FText::FromString(GetGensString(MarketEconomy, PopEconData)));
      PopDetailsWidget->WealthText->SetText(
          FText::FromString(GetWealthString(MarketEconomy->PopEconDataArray, PopEconData)));
      // float PopulationPercent = float(PopEconData.Population) / float(MarketEconomy->TotalPopulation) * 100.0f;
      // PopDetailsWidget->PopulationText->SetText(FText::FromString(
      //     FString::Printf(PopulationPercent < 1.0f ? TEXT("Population: %.1f") : TEXT("Population: %.0f"),
      //                     PopulationPercent) +
      //     "%"));
      PopDetailsWidget->PopulationText->SetText(
          FText::FromString(FString::Printf(TEXT("Population: %d"), PopEconData.Population)));
      PopDetailsWidget->ItemSpendText->SetText(
          FText::FromString("Spends On: " + GetItemSpendString(Pop.ItemSpendPercent)));
      FString EconTypesText =
          Pop.ItemEconTypes.Num() < 4
              ? FString::JoinBy(Pop.ItemEconTypes, TEXT(" & "),
                                [](const EItemEconType& EconType) { return GetItemEconTypeText(EconType).ToString(); })
              : "All";
      PopDetailsWidget->ItemEconTypesText->SetText(FText::FromString("Goods Type: " + EconTypesText));

      if (StatisticsGen->PopStatisticsMap.Num() > 0 && StatisticsGen->PopStatisticsMap.Contains(PopEconData.PopID)) {
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

  GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
  GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UEconomyDetailsWidget::RefreshTick, 3.0f, true,
                                         3.0f);
}

void UEconomyDetailsWidget::InitUI(const class AMarketEconomy* _MarketEconomy,
                                   const class AStatisticsGen* _StatisticsGen) {
  check(_MarketEconomy && _StatisticsGen);

  MarketEconomy = _MarketEconomy;
  StatisticsGen = _StatisticsGen;

  GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
}

void UEconomyDetailsWidget::RefreshTick() {
  if (!TotalInflationGraphWidget || !TotalWealthGraphWidget || !CheckWidgetIsActive(this)) {
    GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
    return;
  }

  if (StatisticsGen->EconomyStatistics.TotalWealthHistory.Num() > 0 &&
      StatisticsGen->EconomyStatistics.TotalBoughtHistory.Num() > 0 &&
      StatisticsGen->EconomyStatistics.TotalInflationHistory.Num() > 0) {
    float TotalWealth = StatisticsGen->EconomyStatistics.TotalWealthHistory.Last();
    TotalWealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), TotalWealth)));
    float TotalBought = StatisticsGen->EconomyStatistics.TotalBoughtHistory.Last();
    TotalBoughtText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), TotalBought)));
    float TotalInflation = StatisticsGen->EconomyStatistics.TotalInflationHistory.Last();
    TotalInflationText->SetText(FText::FromString(FString::Printf(TEXT("%.2f%%"), TotalInflation)));

    TotalWealthGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalWealthHistory);
    TotalWealthGraphWidget->CreateStoreStatsGraph();
    TotalBoughtGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalBoughtHistory);
    TotalBoughtGraphWidget->CreateStoreStatsGraph();
    TotalInflationGraphWidget->InitUI(StatisticsGen->EconomyStatistics.TotalInflationHistory);
    TotalInflationGraphWidget->CreateStoreStatsGraph();
  }

  for (auto* Widget : PopDetailsBox->GetAllChildren()) {
    UPopDetailsWidget* PopDetailsWidget = Cast<UPopDetailsWidget>(Widget);
    if (!PopDetailsWidget) continue;

    const auto SearchedPopEconData = MarketEconomy->PopEconDataArray.FindByPredicate(
        [PopDetailsWidget](const auto& PopEconData) { return PopEconData.PopID == PopDetailsWidget->PopID; });
    if (!SearchedPopEconData) continue;
    const FPopEconData PopEconData = *SearchedPopEconData;

    PopDetailsWidget->GensText->SetText(FText::FromString(GetGensString(MarketEconomy, PopEconData)));
    PopDetailsWidget->WealthText->SetText(
        FText::FromString(GetWealthString(MarketEconomy->PopEconDataArray, PopEconData)));
    PopDetailsWidget->PopulationText->SetText(
        FText::FromString(FString::Printf(TEXT("Population: %d"), PopEconData.Population)));

    if (StatisticsGen->PopStatisticsMap.Num() > 0 && StatisticsGen->PopStatisticsMap.Contains(PopEconData.PopID)) {
      auto GoodsBoughtPerCapitaHistory = StatisticsGen->PopStatisticsMap[PopEconData.PopID].GoodsBoughtPerCapitaHistory;
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
  }

  UE_LOG(LogTemp, Log, TEXT("UEconomyDetailsWidget::RefreshTimer called, visibility: %s"),
         *UEnum::GetValueAsString(GetParent()->GetVisibility()));
}