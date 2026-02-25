#include "DebtManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"

ADebtManager::ADebtManager() {
  PrimaryActorTick.bCanEverTick = false;

  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) { ChangeBehaviorParam(ParamValues); };
}

void ADebtManager::BeginPlay() { Super::BeginPlay(); }
void ADebtManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto ADebtManager::GetInterestRate() const -> float {
  return DebtManagerParams.BaseDebtInterest * BehaviorParams.DebtInterestMulti +
         (DayManager->NextDayToPayDebt - DayManager->CurrentDay) *
             (DebtManagerParams.BaseDebtInterestPerDay * BehaviorParams.BaseDebtInterestPerDayMulti);
}
auto ADebtManager::GetInterest(float Amount) const -> float { return Amount * GetInterestRate(); }
auto ADebtManager::GetDebtLimit() const -> float {
  return StatisticsGen->CachedDetails.PlayerNetWorth * DebtManagerParams.BaseDebtLimit * BehaviorParams.DebtLimitMulti;
}
auto ADebtManager::GetPayableDebt() const -> float { return AdditionalDebt * BehaviorParams.DebtPaymentRequiredMulti; }
auto ADebtManager::GetMaxDebt() const -> float { return GetDebtLimit() - AdditionalDebt + InterestAccrued; }

void ADebtManager::IssueDebt(float Amount) {
  check(Amount <= GetMaxDebt());  // Caller should handle this.

  float Interest = GetInterest(Amount);

  AdditionalDebt += Amount + Interest;
  InterestAccrued += Interest;
  Store->MoneyGained(Amount);
}

auto ADebtManager::TryPayDebt() -> bool {
  float RequiredPayment = FMath::Max(AdditionalDebt * BehaviorParams.DebtPaymentRequiredMulti, 1.0f);
  if (RequiredPayment <= 0) return true;

  if (Store->GetAvailableMoney() >= RequiredPayment) {
    Store->MoneySpent(RequiredPayment);
    AdditionalDebt = FMath::Max(FMath::Floor(AdditionalDebt - RequiredPayment), 0.0f);
    InterestAccrued =
        FMath::Max(FMath::Floor(InterestAccrued - InterestAccrued * BehaviorParams.DebtPaymentRequiredMulti), 0.0f);
    return true;
  }

  return false;
}

void ADebtManager::ChangeBehaviorParam(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    auto StructProp = CastField<FStructProperty>(this->GetClass()->FindPropertyByName("BehaviorParams"));
    auto StructPtr = StructProp->ContainerPtrToValuePtr<void>(this);
    AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}