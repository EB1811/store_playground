#include "SimpleSpriteAnimComponent.h"
#include "PaperFlipbookComponent.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "PaperFlipbook.h"

void USimpleSpriteAnimComponent::BeginPlay() { Super::BeginPlay(); }

void USimpleSpriteAnimComponent::Idle(ESimpleSpriteDirection Direction) {
  SpriteAnimState = ESimpleSpriteAnimState::Idle;
  CurrentDirection = Direction;

  auto* Flipbook = IdleSprites.Find(Direction);
  check(Flipbook);
  UPaperFlipbookComponent* Sprite = GetOwner()->FindComponentByClass<UPaperFlipbookComponent>();
  Sprite->SetFlipbook(*Flipbook);
}

void USimpleSpriteAnimComponent::Walk(ESimpleSpriteDirection Direction) {
  SpriteAnimState = ESimpleSpriteAnimState::Walk;
  CurrentDirection = Direction;

  auto* Flipbook = WalkSprites.Find(Direction);
  check(Flipbook);
  UPaperFlipbookComponent* Sprite = GetOwner()->FindComponentByClass<UPaperFlipbookComponent>();
  Sprite->SetFlipbook(*Flipbook);
}

void USimpleSpriteAnimComponent::MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
  Idle(CurrentDirection);

  UE_LOG(LogTemp, Warning, TEXT("Move completed"));
}