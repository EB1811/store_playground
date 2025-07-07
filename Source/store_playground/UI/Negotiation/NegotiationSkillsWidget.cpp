#include "NegotiationSkillsWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/UI/NpcStore/TradeConfirmWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/UI/Negotiation/NegotiationSkillWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"

void UNegotiationSkillsWidget::InitUI(TArray<FNegotiationSkill> NegotiationSkills) {
  if (NegotiationSkills.Num() == 0) return;

  UE_LOG(LogTemp, Warning, TEXT("Creating NegotiationSkills."));

  UVerticalBoxSlot* FirstSkillWidget = nullptr;
  SkillsBox->ClearChildren();
  for (const FNegotiationSkill& Skill : NegotiationSkills) {
    UNegotiationSkillWidget* SkillWidget = CreateWidget<UNegotiationSkillWidget>(this, NegotiationSkillWidgetClass);

    SkillWidget->SkillName->SetText(Skill.TextData.Name);

    if (!FirstSkillWidget) FirstSkillWidget = SkillsBox->AddChildToVerticalBox(SkillWidget);
    else SkillsBox->AddChildToVerticalBox(SkillWidget);
  }
  FirstSkillWidget->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}