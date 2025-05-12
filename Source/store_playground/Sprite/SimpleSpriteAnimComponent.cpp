#include "SimpleSpriteAnimComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
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
}

void USimpleSpriteAnimComponent::TurnToPlayer(const FVector& PlayerLocation) {
  ESimpleSpriteDirection NewDirection = CurrentDirection;

  float BiggestDelta = 0.0f;
  if (PlayerLocation.X > GetOwner()->GetActorLocation().X &&
      FMath::Abs(PlayerLocation.X - GetOwner()->GetActorLocation().X) > BiggestDelta) {
    NewDirection = ESimpleSpriteDirection::Right;
    BiggestDelta = FMath::Abs(PlayerLocation.X - GetOwner()->GetActorLocation().X);
  }
  if (PlayerLocation.X < GetOwner()->GetActorLocation().X &&
      FMath::Abs(PlayerLocation.X - GetOwner()->GetActorLocation().X) > BiggestDelta) {
    NewDirection = ESimpleSpriteDirection::Left;
    BiggestDelta = FMath::Abs(PlayerLocation.X - GetOwner()->GetActorLocation().X);
  }
  if (PlayerLocation.Y > GetOwner()->GetActorLocation().Y &&
      FMath::Abs(PlayerLocation.Y - GetOwner()->GetActorLocation().Y) > BiggestDelta) {
    NewDirection = ESimpleSpriteDirection::Down;
    BiggestDelta = FMath::Abs(PlayerLocation.Y - GetOwner()->GetActorLocation().Y);
  }
  if (PlayerLocation.Y < GetOwner()->GetActorLocation().Y &&
      FMath::Abs(PlayerLocation.Y - GetOwner()->GetActorLocation().Y) > BiggestDelta) {
    NewDirection = ESimpleSpriteDirection::Up;
    BiggestDelta = FMath::Abs(PlayerLocation.Y - GetOwner()->GetActorLocation().Y);
  }

  LastDirection = CurrentDirection;
  Idle(NewDirection);

  AAIController* OwnerAIController = Cast<APawn>(GetOwner())->GetController<AAIController>();
  if (OwnerAIController) OwnerAIController->StopMovement();

  // if (auto* Path = OwnerAIController->GetPathFollowingComponent())
  //   Path->AbortMove(this, FPathFollowingResultFlags::UserAbort);
}

void USimpleSpriteAnimComponent::ReturnToOgRotation() { Idle(LastDirection); }