#include "EconomyDetailsWidget.h"
#include "Components/WrapBoxSlot.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/UI/Newspaper/PopDetailsWidget.h"
#include "store_playground/UI/Newspaper/PopTypeWrapBox.h"
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
  UE_LOG(LogTemp, Warning, TEXT("GivenPopGoodsBoughtPercentage for %s: %.2f%%"), *GivenPop.PopID.ToString(),
         GivenPopGoodsBoughtPercentage);

  if (GivenPopGoodsBoughtPercentage <= 0.0f) return "None";
  if (GivenPopGoodsBoughtPercentage < 5.0f) return "Very Low";
  if (GivenPopGoodsBoughtPercentage < 10.0f) return "Low";
  if (GivenPopGoodsBoughtPercentage < 20.0f) return "Middle";
  if (GivenPopGoodsBoughtPercentage < 40.0f) return "High";
  if (GivenPopGoodsBoughtPercentage < 60.0f) return "Immense";
  return "Immense";
}
inline auto GetItemSpendText(TMap<EItemWealthType, float> ItemSpendPercent) -> FString {
  TArray<FString> SpendStrings;
  for (const auto& Spend : ItemSpendPercent)
    if (Spend.Value > 40.0f) SpendStrings.Add(UEnum::GetDisplayValueAsText(Spend.Key).ToString());

  return FString::Join(SpendStrings, TEXT(" & "));
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

      // TODO: Add history.
      PopDetailsWidget->Icon->SetBrushFromAtlasInterface(Pop.AssetData.Sprite);
      PopDetailsWidget->Name->SetText(Pop.PopName);
      PopDetailsWidget->WealthText->SetText(
          FText::FromString("Wealth: " + GetWealthText(MarketEconomy->PopEconDataArray, PopEconData)));
      PopDetailsWidget->PopulationText->SetText(FText::FromString(
          FString::Printf(TEXT("Population: %.0f"),
                          float(PopEconData.Population) / float(MarketEconomy->TotalPopulation) * 100.0f) +
          "%"));
      PopDetailsWidget->ItemSpendText->SetText(
          FText::FromString("Spends On: " + GetItemSpendText(Pop.ItemSpendPercent)));
      PopDetailsWidget->ItemEconTypesText->SetText(FText::FromString(
          "Goods Type: " + FString::JoinBy(Pop.ItemEconTypes, TEXT(" & "), [](const EItemEconType& EconType) {
            return UEnum::GetDisplayValueAsText(EconType).ToString();
          })));

      if (!FirstPopDetailsWidget.Key)
        FirstPopDetailsWidget = {PopDetailsWidget, PopDetailsBox->AddChildToWrapBox(PopDetailsWidget)};
      else PopDetailsBox->AddChildToWrapBox(PopDetailsWidget);
    }

    FirstPopDetailsWidget.Key->BgBorder->SetBrushFromMaterial(PopTypeMaterialMap[PopType]);
    FirstPopDetailsWidget.Value->SetNewLine(true);  // Force the first pop details widget to start a new line.
  }
}

void UEconomyDetailsWidget::InitUI(const class AMarketEconomy* _MarketEconomy) {
  check(_MarketEconomy);

  MarketEconomy = _MarketEconomy;
}