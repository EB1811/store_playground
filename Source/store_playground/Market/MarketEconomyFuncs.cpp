#include "MarketEconomy.h"

TArray<float> GetRandomSplit(int32 Buckets, float Value) {
  float Sum = 0;
  TArray<float> RandomMoneyBuckets;
  for (int i = 0; i < Buckets; i++) {
    float RandomRange = FMath::RandRange(0.3f, 0.7f);  // * Avoiding too high of a range.
    RandomMoneyBuckets.Add(RandomRange);
    Sum += RandomRange;
  }

  float scale = Value / Sum;
  for (int i = 0; i < Buckets; i++) RandomMoneyBuckets[i] = std::round(RandomMoneyBuckets[i] * scale);

  return RandomMoneyBuckets;
}