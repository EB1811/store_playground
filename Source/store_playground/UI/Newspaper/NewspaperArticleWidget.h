#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "NewspaperArticleWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNewspaperArticleWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UImage* ArticleImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ArticleTitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ArticleBodyText;

  void SetArticleData(const FArticle& ArticleData);
};
