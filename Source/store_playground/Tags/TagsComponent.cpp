#include "TagsComponent.h"

auto UTagsComponent::GetAllTags() const -> FGameplayTagContainer {
  FGameplayTagContainer AllTags;
  AllTags.AppendTags(ConfigurationTags);
  AllTags.AppendTags(QuestTags);
  AllTags.AppendTags(CutsceneTags);
  return AllTags;
}