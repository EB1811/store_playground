// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/MainMenuPlayer.h"
#include "store_playground/MainMenuControl/MainMenuControlHUD.h"
#include "store_playground/UI/MainMenu/MainMenuWidget.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"
#include "Kismet/GameplayStatics.h"

AMainMenuPlayer::AMainMenuPlayer() {
  // Set this character to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;
}

void AMainMenuPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    // In UI
    EnhancedInputComponent->BindAction(InUIInputActions.AdvanceUIAction, ETriggerEvent::Triggered, this,
                                       &AMainMenuPlayer::AdvanceUI);
    EnhancedInputComponent->BindAction(InUIInputActions.RetractUIAction, ETriggerEvent::Triggered, this,
                                       &AMainMenuPlayer::RetractUIAction);
    EnhancedInputComponent->BindAction(InUIInputActions.UISideButton4Action, ETriggerEvent::Triggered, this,
                                       &AMainMenuPlayer::UISideButton4Action);
    EnhancedInputComponent->BindAction(InUIInputActions.UIDirectionalInputAction, ETriggerEvent::Triggered, this,
                                       &AMainMenuPlayer::UIDirectionalInputAction);
  }
}

void AMainMenuPlayer::BeginPlay() {
  Super::BeginPlay();

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->AddMappingContext(UIInputContext, 0);

  HUD = Cast<AMainMenuControlHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  HUD->InUIInputActions = InUIInputActions;
}

void AMainMenuPlayer::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AMainMenuPlayer::AdvanceUI(const FInputActionValue& Value) { HUD->AdvanceUI(); }
void AMainMenuPlayer::RetractUIAction(const FInputActionValue& Value) { HUD->RetractUIAction(); }
void AMainMenuPlayer::UIDirectionalInputAction(const FInputActionValue& Value) {
  HUD->UIDirectionalInputAction(Value.Get<FVector2D>());
}
void AMainMenuPlayer::UISideButton4Action(const FInputActionValue& Value) { HUD->UISideButton4Action(); }