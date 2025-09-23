// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/Player/PlayerZDCharacter.h"
#include <cstddef>
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Internationalization/Text.h"
#include "Logging/LogVerbosity.h"
#include "Materials/MaterialInstance.h"
#include "Misc/AssertionMacros.h"
#include "PaperZDCharacter.h"
#include "PlayerZDCharacter.h"
#include "TimerManager.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Tutorial/TutorialManager.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Pickup/PickupComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/AIStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/WorldObject/Level/SpawnPoint.h"
#include "store_playground/Level/LevelChangeComponent.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "store_playground/Lighting/StorePhaseLightingManager.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Tags/TagsComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "store_playground/UI/InGameHud/InGameHudWidget.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

APlayerZDCharacter::APlayerZDCharacter() {
  // Set this character to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  PlayerBehaviourState = EPlayerState::Normal;

  OcclusionCheckData.LastOcclusionCheckTime = 0.0f;
  OcclusionCheckData.OcclusionCheckFrequency = 200.0f;

  InteractionData.InteractionCheckFrequency = 0.0f;
  InteractionData.InteractionCheckDistance = 200.0f;
  InteractionData.InteractionCheckRadius = 50.0f;

  SpringArmC = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
  SpringArmC->SetupAttachment(RootComponent);
  CameraC = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
  CameraC->SetupAttachment(SpringArmC);

  PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  PlayerTagsComponent = CreateDefaultSubobject<UTagsComponent>(TEXT("TagsComponent"));
  PlayerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerWidgetComponent"));
  FootstepAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudio"));
}

void APlayerZDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    // In Game
    EnhancedInputComponent->BindAction(InGameInputActions.MoveAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::Move);
    EnhancedInputComponent->BindAction(InGameInputActions.MoveAction, ETriggerEvent::Started, this,
                                       &APlayerZDCharacter::StartMove);
    EnhancedInputComponent->BindAction(InGameInputActions.MoveAction, ETriggerEvent::Completed, this,
                                       &APlayerZDCharacter::StopMove);
    EnhancedInputComponent->BindAction(InGameInputActions.OpenPauseMenuAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenPauseMenu);
    EnhancedInputComponent->BindAction(InGameInputActions.OpenInventoryViewAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenInventoryView);
    EnhancedInputComponent->BindAction(InGameInputActions.BuildModeAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::EnterBuildMode);
    EnhancedInputComponent->BindAction(InGameInputActions.OpenNewspaperAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenNewspaper);
    EnhancedInputComponent->BindAction(InGameInputActions.OpenStoreViewAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::OpenStoreView);
    EnhancedInputComponent->BindAction(InGameInputActions.InteractAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::Interact);
    EnhancedInputComponent->BindAction(InGameInputActions.CinematicViewAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::CinematicView);
    // In UI
    EnhancedInputComponent->BindAction(InUIInputActions.AdvanceUIAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::AdvanceUI);
    EnhancedInputComponent->BindAction(InUIInputActions.AdvanceUIHoldAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::AdvanceUIHold);
    EnhancedInputComponent->BindAction(InUIInputActions.RetractUIAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::RetractUIAction);
    EnhancedInputComponent->BindAction(InUIInputActions.QuitUIAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::QuitUIAction);
    EnhancedInputComponent->BindAction(InUIInputActions.UINumericInputAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UINumericInputAction);
    EnhancedInputComponent->BindAction(InUIInputActions.UIDirectionalInputAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UIDirectionalInputAction);
    EnhancedInputComponent->BindAction(InUIInputActions.UISideButton1Action, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UISideButton1Action);
    EnhancedInputComponent->BindAction(InUIInputActions.UISideButton2Action, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UISideButton2Action);
    EnhancedInputComponent->BindAction(InUIInputActions.UISideButton3Action, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UISideButton3Action);
    EnhancedInputComponent->BindAction(InUIInputActions.UISideButton4Action, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UISideButton4Action);
    EnhancedInputComponent->BindAction(InUIInputActions.UICycleLeftAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UICycleLeftAction);
    EnhancedInputComponent->BindAction(InUIInputActions.UICycleRightAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::UICycleRightAction);

    // In Cutscene
    EnhancedInputComponent->BindAction(InCutsceneInputActions.AdvanceCutsceneAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::AdvanceUI);
    EnhancedInputComponent->BindAction(InCutsceneInputActions.SkipCutsceneAction, ETriggerEvent::Triggered, this,
                                       &APlayerZDCharacter::SkipCutscene);
  }
}

void APlayerZDCharacter::BeginPlay() {
  Super::BeginPlay();

  PlayerBehaviourState = EPlayerState::NoControl;
  GameActionsState = EPlayerGameActionsState::None;

  // Make sure all player data is correctly initialized.
  check(PlayerInventoryComponent->InventoryType == EInventoryType::PlayerInventory);
  check(SpawnPointClass);

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->AddMappingContext(InputContexts[EPlayerState::Normal], 0);

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  HUD->SetPlayerFocussedFunc = [this]() {
    // ? Set these states in player?
    if (PlayerBehaviourState == EPlayerState::Cutscene) return;
    ChangePlayerState(EPlayerState::FocussedMenu);
  };
  HUD->SetPlayerNormalFunc = [this]() {
    // ? Set these states in player?
    if (PlayerBehaviourState == EPlayerState::Cutscene) return;
    if (PlayerBehaviourState == EPlayerState::PausedCutscene) return ChangePlayerState(EPlayerState::Cutscene);

    ChangePlayerState(EPlayerState::Normal);
  };
  HUD->SetPlayerNoControlFunc = [this]() { ChangePlayerState(EPlayerState::NoControl); };
  HUD->SetPlayerPausedFunc = [this]() {
    // ? Set these states in player?
    if (PlayerBehaviourState == EPlayerState::Cutscene) return ChangePlayerState(EPlayerState::PausedCutscene);
    ChangePlayerState(EPlayerState::Paused);
  };
  // Input actions
  HUD->InGameInputActions = InGameInputActions;
  HUD->InUIInputActions = InUIInputActions;
  HUD->InCutsceneInputActions = InCutsceneInputActions;

  CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

  auto* PlayerController = Cast<APlayerController>(GetController());
  PlayerController->SetAudioListenerOverride(
      nullptr, GetRootComponent()->GetComponentLocation() + FVector(0, 0, 200.0f), CameraC->GetComponentRotation());

  // Cinematics
  CameraCinematicsData.OgCamRotation = CameraC->GetRelativeRotation();
  CameraCinematicsData.OgSpringArmOffset = SpringArmC->TargetOffset;

  // Widget Component
  PlayerWidgetComponent->SetVisibility(false, true);
}

void APlayerZDCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  // ? Could check for movement before, but not worth potentially missing a frame / dealing with input lag.

  if (PlayerBehaviourState == EPlayerState::Normal &&
      GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionData.InteractionCheckFrequency)
    CheckForInteraction();

  if (PlayerBehaviourState == EPlayerState::Normal &&
      GetWorld()->TimeSince(OcclusionCheckData.LastOcclusionCheckTime) > OcclusionCheckData.OcclusionCheckFrequency)
    HandleOcclusion();

  if (PlayerBehaviourState == EPlayerState::Normal &&
      GetWorld()->TimeSince(LastCurrentFootstepPhysMatCheckTime) > InteractionData.InteractionCheckFrequency)
    CheckCurrentFootstepPhysMat();

  if (PlayerBehaviourState != EPlayerState::Paused && PlayerBehaviourState != EPlayerState::PausedCutscene)
    InterpCamera(DeltaTime);

  if (PlayerBehaviourState == EPlayerState::Normal && PlayerWidgetComponent->IsWidgetVisible())
    PlayerWidgetComponent->SetWorldLocation(GetActorLocation() + FVector(0, -35.0f, 110.0f));

  auto* PlayerController = Cast<APlayerController>(GetController());
  PlayerController->SetAudioListenerOverride(
      nullptr, GetRootComponent()->GetComponentLocation() + FVector(0, 0, 200.0f), CameraC->GetComponentRotation());
}

void APlayerZDCharacter::ChangePlayerState(EPlayerState NewState) {
  check(PlayerBehaviourState != EPlayerState::GameOver);
  if (PlayerBehaviourState == NewState) return;

  if (NewState == EPlayerState::Paused || NewState == EPlayerState::PausedCutscene)
    UGameplayStatics::SetGamePaused(GetWorld(), true);
  if ((PlayerBehaviourState == EPlayerState::Paused || PlayerBehaviourState == EPlayerState::PausedCutscene) &&
      (NewState != EPlayerState::Paused || NewState != EPlayerState::PausedCutscene))
    UGameplayStatics::SetGamePaused(GetWorld(), false);

  UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  Subsystem->RemoveMappingContext(InputContexts[PlayerBehaviourState]);
  Subsystem->AddMappingContext(InputContexts[NewState], 0);

  EPlayerState OldState = PlayerBehaviourState;
  PlayerBehaviourState = NewState;
  UE_LOG(LogTemp, Log, TEXT("Player state changed to: %s"), *UEnum::GetDisplayValueAsText(NewState).ToString());

  if (StorePhaseManager->StorePhaseState == EStorePhaseState::ShopOpen)
    StorePhaseManager->ShopOpenConsiderPlayerState(PlayerBehaviourState);

  if (PlayerBehaviourState == EPlayerState::Cutscene) {
    // ToggleCinematicView();
    HUD->HideInGameHudWidget();
  }
  if (NewState == EPlayerState::PausedCutscene) CutsceneSystem->PauseCutscene();
  else if (NewState == EPlayerState::Cutscene && OldState == EPlayerState::PausedCutscene)
    CutsceneSystem->ResumeCutscene();

  // Showing tutorials when available.
  if (PlayerBehaviourState == EPlayerState::Normal) TutorialManager->ShowPendingTutorials();

  // Informing game actions state of player.
  if (PlayerBehaviourState == EPlayerState::Normal) ChangeGameActionsState(EPlayerGameActionsState::None);
}
void APlayerZDCharacter::ChangeGameActionsState(EPlayerGameActionsState NewState) {
  if (GameActionsState == NewState) return;

  GameActionsState = NewState;
  UE_LOG(LogTemp, Log, TEXT("Player game actions state changed to: %s"),
         *UEnum::GetDisplayValueAsText(NewState).ToString());
}

void APlayerZDCharacter::Move(const FInputActionValue& Value) {
  const FVector2D MovementVector = Value.Get<FVector2D>();
  const FRotator Rotation = GetControlRotation();
  const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
  const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
  const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

  FacingDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
  FacingDirection.Normalize();

  AddMovementInput(ForwardDirection, MovementVector.Y);
  AddMovementInput(RightDirection, MovementVector.X);

  FRotator NewRotation = FQuat::FindBetweenVectors(FVector::ForwardVector, ForwardDirection * MovementVector.Y +
                                                                               RightDirection * MovementVector.X)
                             .Rotator();
  NewRotation.Pitch = 0.0f;
  NewRotation.Roll = 0.0f;
  NewRotation.Normalize();
  SetActorRotation(NewRotation);
}
void APlayerZDCharacter::StartMove(const FInputActionValue& Value) {
  FootstepAudio->Activate();
  GetWorld()->GetTimerManager().SetTimer(FootstepAudioTimer, this, &APlayerZDCharacter::PlayFootstepAudio, 0.32f, true);
}
void APlayerZDCharacter::StopMove(const FInputActionValue& Value) {
  // FootstepAudio->Deactivate();
  GetWorld()->GetTimerManager().ClearTimer(FootstepAudioTimer);
}

void APlayerZDCharacter::OpenPauseMenu(const FInputActionValue& Value) {
  if (bInCinematicView) ToggleCinematicView();

  switch (PlayerBehaviourState) {
    case EPlayerState::Normal: ChangePlayerState(EPlayerState::Paused); break;
    case EPlayerState::Cutscene: ChangePlayerState(EPlayerState::PausedCutscene); break;
    case EPlayerState::Paused:
    case EPlayerState::PausedCutscene: break;

    case EPlayerState::FocussedMenu:
    case EPlayerState::NoControl:
    case EPlayerState::GameOver: checkNoEntry();
    default: checkNoEntry();
  }

  HUD->OpenPauseMenuView();
}

void APlayerZDCharacter::OpenInventoryView(const FInputActionValue& Value) {
  if (bInCinematicView) ToggleCinematicView();

  if (PlayerBehaviourState != EPlayerState::Normal) return;

  HUD->SetAndOpenInventoryView(PlayerInventoryComponent);
}
void APlayerZDCharacter::OpenNewspaper(const FInputActionValue& Value) {
  if (bInCinematicView) ToggleCinematicView();

  if (PlayerBehaviourState != EPlayerState::Normal) return;

  HUD->SetAndOpenNewsAndEconomyView();

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.NewsEconomyView"));
}
void APlayerZDCharacter::OpenStoreView(const FInputActionValue& Value) {
  if (bInCinematicView) ToggleCinematicView();

  if (PlayerBehaviourState != EPlayerState::Normal) return;

  HUD->SetAndOpenStoreView(PlayerInventoryComponent);

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.StoreView"));
}

void APlayerZDCharacter::EnterBuildMode(const FInputActionValue& Value) {
  if (bInCinematicView) ToggleCinematicView();

  if (PlayerBehaviourState != EPlayerState::Normal) return;

  if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning &&
      StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode)
    return;

  if (LevelManager->CurrentLevel != ELevel::Store) return;

  StorePhaseManager->EnterBuildMode();

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.BuildMode"));
}
// Rechecking on input to avoid problems with the interaction frequency not keeping up.
void APlayerZDCharacter::Interact(const FInputActionValue& Value) {
  if (PlayerBehaviourState != EPlayerState::Normal) return;

  if (CheckForInteraction()) HandleInteraction(CurrentInteractableC);

  if (bInCinematicView) ToggleCinematicView();
}

void APlayerZDCharacter::CinematicView(const FInputActionValue& Value) {
  if (PlayerBehaviourState != EPlayerState::Normal) return;

  ToggleCinematicView();
}

void APlayerZDCharacter::AdvanceUI(const FInputActionValue& Value) { HUD->AdvanceUI(); }
void APlayerZDCharacter::AdvanceUIHold(const FInputActionValue& Value) { HUD->AdvanceUIHold(); }
void APlayerZDCharacter::RetractUIAction(const FInputActionValue& Value) { HUD->RetractUIAction(); }
void APlayerZDCharacter::QuitUIAction(const FInputActionValue& Value) { HUD->QuitUIAction(); }
void APlayerZDCharacter::UINumericInputAction(const FInputActionValue& Value) {
  HUD->UINumericInputAction(Value.Get<float>());
}
void APlayerZDCharacter::UIDirectionalInputAction(const FInputActionValue& Value) {
  HUD->UIDirectionalInputAction(Value.Get<FVector2D>());
}
void APlayerZDCharacter::UISideButton1Action(const FInputActionValue& Value) { HUD->UISideButton1Action(); }
void APlayerZDCharacter::UISideButton2Action(const FInputActionValue& Value) { HUD->UISideButton2Action(); }
void APlayerZDCharacter::UISideButton3Action(const FInputActionValue& Value) { HUD->UISideButton3Action(); }
void APlayerZDCharacter::UISideButton4Action(const FInputActionValue& Value) { HUD->UISideButton4Action(); }
void APlayerZDCharacter::UICycleLeftAction(const FInputActionValue& Value) { HUD->UICycleLeftAction(); }
void APlayerZDCharacter::UICycleRightAction(const FInputActionValue& Value) { HUD->UICycleRightAction(); }

void APlayerZDCharacter::SkipCutscene(const FInputActionValue& Value) {
  if (PlayerBehaviourState != EPlayerState::Cutscene) return;

  CutsceneSystem->SkipCutsceneChain();
}

void APlayerZDCharacter::PlayFootstepAudio() {
  if (PlayerBehaviourState != EPlayerState::Normal) return;

  if (CurrentFootstepPhysMat) FootstepAudio->SetIntParameter(FName("SurfaceType"), CurrentFootstepPhysMat->SurfaceType);
  FootstepAudio->Play();
}
void APlayerZDCharacter::CheckCurrentFootstepPhysMat() {
  LastCurrentFootstepPhysMatCheckTime = GetWorld()->GetTimeSeconds();

  // pawn to ground
  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart - FVector(0, 0, 500)};

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  TraceParams.bReturnPhysicalMaterial = true;  // Why is this not default?
  FHitResult TraceHit;

  if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
    if (auto PhysMaterial = TraceHit.PhysMaterial.Get()) {
      if (CurrentFootstepPhysMat != PhysMaterial) CurrentFootstepPhysMat = PhysMaterial;
    } else {
      UE_LOG(LogTemp, Log, TEXT("No physical material found for footstep sound."));
    }
  }
}

void APlayerZDCharacter::HandleOcclusion() {
  OcclusionCheckData.LastOcclusionCheckTime = GetWorld()->GetTimeSeconds();

  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{CameraManager->GetCameraLocation()};
  FCollisionShape Sphere = FCollisionShape::MakeSphere(25.0f);

  // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  TraceParams.AddIgnoredActor(CameraManager->GetViewTarget());
  FHitResult TraceHit;

  if (GetWorld()->SweepSingleByChannel(TraceHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere,
                                       TraceParams)) {
    if (auto* HitActor = TraceHit.GetActor()) {
      if (!HitActor->ActorHasTag(OcclusionCheckData.OcclusionTag)) return;

      if (OccludedActors.Contains(HitActor)) return;
      OccludedActors.AddUnique(HitActor);

      // HitActor->SetActorHiddenInGame(true);
      TArray<UStaticMeshComponent*> Components;
      HitActor->GetComponents<UStaticMeshComponent>(Components);
      for (auto* StaticMeshComponent : Components) {
        StaticMeshComponent->bCastHiddenShadow = true;
        StaticMeshComponent->bAffectIndirectLightingWhileHidden = true;
        StaticMeshComponent->SetHiddenInGame(true);
      }

      // ! Hiding for now.
      // * Need a way to save each of the original materials.
      // OccludedActors.AddUnique(HitActor);
      // TArray<UStaticMeshComponent*> Components;
      // HitActor->GetComponents<UStaticMeshComponent>(Components);
      // for (auto* StaticMeshComponent : Components)
      //   for (int i = 0; i < StaticMeshComponent->GetStaticMesh()->GetStaticMaterials().Num(); i++)
      //     StaticMeshComponent->SetMaterial(i, OcclusionCheckData.OcclusionMaterial);
    }
  } else {
    for (auto* OccludedActor : OccludedActors)
      if (OccludedActor) {
        TArray<UStaticMeshComponent*> Components;
        OccludedActor->GetComponents<UStaticMeshComponent>(Components);
        for (auto* StaticMeshComponent : Components) StaticMeshComponent->SetHiddenInGame(false);
      }
    OccludedActors.Empty();
  }
}

void APlayerZDCharacter::ToggleCinematicView() {
  if (bInCinematicView) {
    HUD->ShowInGameHudWidget();

    bInCinematicView = false;
  } else {
    HUD->HideInGameHudWidget();

    bInCinematicView = true;
  }
}
void APlayerZDCharacter::InterpCamera(float DeltaTime) {
  if (!bInterpCamera) {
    SpringArmC->SetRelativeLocation(GetActorLocation());
    return;
  }

  // * Camera lag.
  if (!SpringArmC->GetRelativeLocation().Equals(GetActorLocation(), 0.01f)) {
    FVector NewLocation = FMath::VInterpTo(SpringArmC->GetRelativeLocation(), GetActorLocation(), DeltaTime,
                                           CameraCinematicsData.CamInterpSpeed);
    SpringArmC->SetRelativeLocation(NewLocation);
  }

  if (bInCinematicView) {
    FRotator TargetRotation = CameraCinematicsData.OgCamRotation + CameraCinematicsData.CinematicCamRotation;
    FVector TargetOffset = CameraCinematicsData.OgSpringArmOffset + CameraCinematicsData.CinematicSpringArmOffset;
    if (CameraC->GetRelativeRotation().Equals(TargetRotation, 0.01f) &&
        SpringArmC->TargetOffset.Equals(TargetOffset, 0.01f))
      return;

    FRotator NewRotation = FMath::RInterpTo(CameraC->GetRelativeRotation(), TargetRotation, DeltaTime,
                                            CameraCinematicsData.CinematicInterpSpeed);
    CameraC->SetRelativeRotation(NewRotation);

    FVector NewOffset =
        FMath::VInterpTo(SpringArmC->TargetOffset, TargetOffset, DeltaTime, CameraCinematicsData.CinematicInterpSpeed);
    SpringArmC->TargetOffset = NewOffset;

  } else {
    if (CameraC->GetRelativeRotation().Equals(CameraCinematicsData.OgCamRotation, 0.01f) &&
        SpringArmC->TargetOffset.Equals(CameraCinematicsData.OgSpringArmOffset, 0.01f))
      return;

    FRotator NewRotation = FMath::RInterpTo(CameraC->GetRelativeRotation(), CameraCinematicsData.OgCamRotation,
                                            DeltaTime, CameraCinematicsData.CinematicInterpSpeed);
    CameraC->SetRelativeRotation(NewRotation);

    FVector NewOffset = FMath::VInterpTo(SpringArmC->TargetOffset, CameraCinematicsData.OgSpringArmOffset, DeltaTime,
                                         CameraCinematicsData.CinematicInterpSpeed);
    SpringArmC->TargetOffset = NewOffset;
  }
}

auto APlayerZDCharacter::CheckForInteraction() -> bool {
  InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

  FVector TraceStart{GetPawnViewLocation() - FVector(0, 0, 50)};
  FVector TraceEnd{TraceStart + FacingDirection * InteractionData.InteractionCheckDistance};
  FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionData.InteractionCheckRadius);

  // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f, 0, 5.0f);

  FCollisionQueryParams TraceParams;
  TraceParams.AddIgnoredActor(this);
  FHitResult TraceHit;

  if (GetWorld()->SweepSingleByChannel(TraceHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, Sphere,
                                       TraceParams)) {
    if ((TraceStart - TraceHit.ImpactPoint).Size() <= InteractionData.InteractionCheckDistance)
      if (UInteractionComponent* Interactable = TraceHit.GetActor()->FindComponentByClass<UInteractionComponent>()) {
        if (!IsInteractable(Interactable)) return false;

        CurrentInteractableC = Interactable;
        if (!bInCinematicView) {
          PlayerWidgetComponent->SetWorldLocation(GetActorLocation() + FVector(0, -35.0f, 110.0f));
          PlayerWidgetComponent->SetVisibility(true, true);
        }
        return true;
      }
  }

  if (CurrentInteractableC) CurrentInteractableC = nullptr;
  PlayerWidgetComponent->SetVisibility(false, true);

  return false;
}

auto APlayerZDCharacter::IsInteractable(const UInteractionComponent* Interactable) const -> bool {
  if (PlayerBehaviourState != EPlayerState::Normal) return false;

  switch (Interactable->InteractionType) {
    case EInteractionType::None: return false; break;
    case EInteractionType::Buildable:
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode) return false;
      break;
    case EInteractionType::StockDisplay:
      if (StorePhaseManager->StorePhaseState != EStorePhaseState::Morning &&
          StorePhaseManager->StorePhaseState != EStorePhaseState::Night)
        return false;
      break;
    case EInteractionType::LevelChange:
    case EInteractionType::LeaveStore:
      if (StorePhaseManager->StorePhaseState == EStorePhaseState::ShopOpen) return false;
      break;
  }

  return true;
}

// ? Move to system?
void APlayerZDCharacter::HandleInteraction(UInteractionComponent* Interactable) {
  PlayerWidgetComponent->SetVisibility(false, true);

  Interactable->PlayInteractionSound();
  switch (Interactable->InteractionType) {
    case EInteractionType::LevelChange: {
      auto LevelChangeC = Interactable->InteractLevelChange();

      EnterNewLevel(LevelChangeC);
      break;
    }
    case EInteractionType::LeaveStore: {
      LeaveStore();
      break;
    }
    case EInteractionType::StoreNextPhase: {
      StorePhaseManager->NextPhase();
      break;
    }
    case EInteractionType::UpgradeSelect: {
      auto UpgradeSelectC = Interactable->InteractUpgradeSelect();
      EnterUpgradeSelect(UpgradeSelectC);
      break;
    }
    case EInteractionType::AbilitySelect: {
      EnterAbilitySelect();
      break;
    }
    case EInteractionType::Use: {
      Interactable->InteractUse();
      break;
    }
    case EInteractionType::Buildable: {
      auto Buildable = Interactable->InteractBuildable();
      EnterBuildableDisplay(Buildable.GetValue());
      break;
    }
    case EInteractionType::StockDisplay: {
      auto [DisplayC, DisplayInventoryC] = Interactable->InteractStockDisplay();
      EnterStockDisplay(DisplayC, DisplayInventoryC);
      break;
    }
    case EInteractionType::NPCDialogue: {
      auto [DialogueC, SpriteAnimC] = Interactable->InteractNPCDialogue();

      SpriteAnimC->TurnToPlayer(GetActorLocation());
      EnterDialogue(DialogueC, [this, SpriteAnimC]() { SpriteAnimC->ReturnToOgRotation(); });
      break;
    }
    case EInteractionType::Customer: {
      auto [DialogueC, CustomerAI, SpriteAnimC] = Interactable->InteractCustomer();

      SpriteAnimC->TurnToPlayer(GetActorLocation());
      CustomerAI->StartDialogue();
      EnterDialogue(DialogueC, [this, CustomerAI, SpriteAnimC]() { CustomerAI->LeaveDialogue(); });
      break;
    }
    case EInteractionType::WaitingCustomer: {
      auto [CustomerAI, Item, SpriteAnimC] = Interactable->InteractWaitingCustomer();

      SpriteAnimC->TurnToPlayer(GetActorLocation());
      EnterNegotiation(CustomerAI, Item);
      break;
    }
    case EInteractionType::UniqueNPCQuest: {
      auto [Dialogue, QuestC, CustomerAI, Item, SpriteAnimC] = Interactable->InteractUniqueNPCQuest();

      SpriteAnimC->TurnToPlayer(GetActorLocation());
      if (CustomerAI) CustomerAI->StartQuest();
      EnterQuest(QuestC, Dialogue, SpriteAnimC, CustomerAI, Item);
      break;
    }
    case EInteractionType::NpcStore: {
      auto [NpcStoreC, StoreInventory, DialogueC] = Interactable->InteractNpcStore();
      EnterDialogue(DialogueC, nullptr,
                    [this, NpcStoreC, StoreInventory]() { EnterNpcStore(NpcStoreC, StoreInventory); });
      break;
    }
    case EInteractionType::Pickup: {
      auto PickupC = Interactable->InteractPickup();
      Pickup(PickupC.GetValue());
      break;
    }
    case EInteractionType::MiniGame: {
      auto [MiniGameC, DialogueC] = Interactable->InteractMiniGame();
      EnterDialogue(DialogueC, nullptr, [this, MiniGameC]() { EnterMiniGame(MiniGameC); });
      break;
    }
    default: checkNoEntry();
  }
}

void APlayerZDCharacter::SetupNpcInteraction(USimpleSpriteAnimComponent* SpriteAnimC) {}
void APlayerZDCharacter::SetupCustomerInteraction(UCustomerAIComponent* CustomerAI,
                                                  USimpleSpriteAnimComponent* SpriteAnimC) {}

void APlayerZDCharacter::EnterBuildableDisplay(ABuildable* Buildable) { HUD->SetAndOpenBuildableDisplay(Buildable); }

void APlayerZDCharacter::EnterStockDisplay(UStockDisplayComponent* StockDisplayC,
                                           UInventoryComponent* DisplayInventoryC) {
  HUD->SetAndOpenStockDisplay(StockDisplayC, DisplayInventoryC, PlayerInventoryComponent);

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.StockDisplay"));
}

void APlayerZDCharacter::EnterUpgradeSelect(UUpgradeSelectComponent* UpgradeSelectC) {
  HUD->SetAndOpenUpgradeView(UpgradeSelectC);

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.UpgradeSelect"));
}

void APlayerZDCharacter::EnterAbilitySelect() {
  HUD->SetAndOpenAbilityView();

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.AbilitySelect"));
}

void APlayerZDCharacter::EnterMiniGame(UMiniGameComponent* MiniGameC) {
  HUD->SetAndOpenMiniGame(MiniGameManager, MiniGameC, Store, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterNpcStore(UNpcStoreComponent* NpcStoreC, UInventoryComponent* StoreInventoryC) {
  HUD->SetAndOpenNPCStore(NpcStoreC, StoreInventoryC, PlayerInventoryComponent);
}

void APlayerZDCharacter::EnterDialogue(UDialogueComponent* DialogueC,
                                       std::function<void()> OnDialogueCloseFunc,
                                       std::function<void()> OnDialogueFinishFunc) {
  if (DialogueC->DialogueArray.Num() == 0) {
    if (OnDialogueCloseFunc) OnDialogueCloseFunc();
    if (OnDialogueFinishFunc) OnDialogueFinishFunc();
    return;
  }

  DialogueSystem->StartDialogue(DialogueC);
  HUD->SetAndOpenDialogue(DialogueSystem, OnDialogueCloseFunc, OnDialogueFinishFunc);
}
// For dialogue outside of the dialogue component (cutscenes, etc.).
void APlayerZDCharacter::EnterDialogue(const TArray<FDialogueData> DialogueDataArr,
                                       std::function<void()> OnDialogueCloseFunc,
                                       std::function<void()> OnDialogueFinishFunc,
                                       const FString& _SpeakerName) {
  if (DialogueDataArr.Num() == 0) {
    if (OnDialogueCloseFunc) OnDialogueCloseFunc();
    if (OnDialogueFinishFunc) OnDialogueFinishFunc();
    return;
  }

  DialogueSystem->StartDialogue(DialogueDataArr, _SpeakerName);
  HUD->SetAndOpenDialogue(DialogueSystem, OnDialogueCloseFunc, OnDialogueFinishFunc);
}

void APlayerZDCharacter::EnterNegotiation(UCustomerAIComponent* CustomerAI,
                                          UItemBase* Item,
                                          bool bIsQuestAssociated,
                                          UQuestComponent* QuestComponent) {
  CustomerAI->StartRequestDialogue();

  NegotiationSystem->StartNegotiation(CustomerAI, Item, CustomerAI->NegotiationAIDetails.StockDisplayInventory,
                                      bIsQuestAssociated, QuestComponent);
  HUD->SetAndOpenNegotiation(NegotiationSystem, DialogueSystem, PlayerInventoryComponent);

  TutorialManager->CheckAndShowTutorial(FGameplayTag::RequestGameplayTag("Tutorial.NegotiationView"));
}

void APlayerZDCharacter::EnterQuest(UQuestComponent* QuestC,
                                    UDialogueComponent* DialogueC,
                                    USimpleSpriteAnimComponent* SpriteAnimC,
                                    UCustomerAIComponent* CustomerAI,
                                    UItemBase* Item) {
  ChangeGameActionsState(EPlayerGameActionsState::InQuest);

  // * Differentiate between when quest is from a customer (store open), and from a npc (e.g., in market).
  if (!CustomerAI)
    return EnterDialogue(
        DialogueC, [SpriteAnimC]() { SpriteAnimC->ReturnToOgRotation(); },
        [this, QuestC, SpriteAnimC, CustomerAI]() {
          QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
        });

  switch (QuestC->CustomerAction) {
    case ECustomerAction::PickItem:
    case ECustomerAction::StockCheck:
    case ECustomerAction::SellItem:
      check(CustomerAI->NegotiationAIDetails.RelevantItem);
      if (QuestC->QuestOutcomeType == EQuestOutcomeType::Negotiation)
        EnterDialogue(
            DialogueC, [CustomerAI, SpriteAnimC]() { CustomerAI->LeaveQuest(); },
            [this, Item, CustomerAI, QuestC]() { EnterNegotiation(CustomerAI, Item, true, QuestC); });
      else
        EnterDialogue(
            DialogueC, [CustomerAI, SpriteAnimC]() { CustomerAI->LeaveQuest(); },
            [this, QuestC, CustomerAI, Item]() {
              QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
              EnterNegotiation(CustomerAI, Item);
            });
      break;
    case ECustomerAction::Leave:
    case ECustomerAction::None:
      if (CustomerAI->CustomerState == ECustomerState::Leaving) return;
      EnterDialogue(
          DialogueC, [CustomerAI, SpriteAnimC]() { CustomerAI->LeaveQuest(); },
          [this, QuestC, CustomerAI]() {
            QuestManager->CompleteQuestChain(QuestC, DialogueSystem->ChoiceDialoguesSelectedIDs);
            CustomerAI->FinishQuest();
          });
      break;
    default: checkNoEntry(); break;
  }
}

void APlayerZDCharacter::EnterCutscene(const FResolvedCutsceneData ResolvedCutsceneData) {
  UE_LOG(LogTemp, Warning, TEXT("Entering cutscene: %s"), *ResolvedCutsceneData.CutsceneData.ID.ToString());

  ChangePlayerState(EPlayerState::Cutscene);
  CutsceneSystem->StartCutscene(ResolvedCutsceneData, [this]() {
    UE_LOG(LogTemp, Warning, TEXT("Cutscene finished"));
    ChangePlayerState(EPlayerState::Normal);
  });
}

void APlayerZDCharacter::Pickup(UPickupComponent* PickupC) {
  switch (PickupC->PickupGoodType) {
    case EPickupGoodType::Item: {
      UItemBase* Item = Market->GetItem(PickupC->ItemID);
      PlayerInventoryComponent->AddItem(Item, PickupC->ItemQuantity);

      // ? Move to global data manager?
      FText ItemName = Item->TextData.Name;
      TArray<FDialogueData> DialogueArray = PickupC->DialogueArray;
      TArray<FDialogueData> NewDialogueArray = {};
      NewDialogueArray.Reserve(DialogueArray.Num());
      for (const FDialogueData& Dialogue : DialogueArray) {
        FString NewDialogue = Dialogue.DialogueText.ToString();
        NewDialogue = NewDialogue.Replace(TEXT("{Pickup}"), *ItemName.ToString());

        FDialogueData NewDialogueData = Dialogue;
        NewDialogueData.DialogueText = FText::FromString(NewDialogue);
        NewDialogueArray.Add(NewDialogueData);
      }

      PickupC->DestroyPickup();
      EnterDialogue(NewDialogueArray);
      break;
    }
    case EPickupGoodType::Money: {
      Store->MoneyGained(PickupC->MoneyAmount);

      // ? Move to global data manager?
      TArray<FDialogueData> DialogueArray = PickupC->DialogueArray;
      TArray<FDialogueData> NewDialogueArray = {};
      NewDialogueArray.Reserve(DialogueArray.Num());
      for (const FDialogueData& Dialogue : DialogueArray) {
        FString NewDialogue = Dialogue.DialogueText.ToString();
        NewDialogue = NewDialogue.Replace(TEXT("{Pickup}"), *FString::Printf(TEXT("%0.0f¬"), PickupC->MoneyAmount));

        FDialogueData NewDialogueData = Dialogue;
        NewDialogueData.DialogueText = FText::FromString(NewDialogue);
        NewDialogueArray.Add(NewDialogueData);
      }

      PickupC->DestroyPickup();
      EnterDialogue(NewDialogueArray);
      break;
    }
    default: checkNoEntry(); break;
  }
}

// ? All player states handled?
void APlayerZDCharacter::ExitCurrentAction() {
  if (PlayerBehaviourState == EPlayerState::Normal) return;

  // * Should never be called in cutscene state.
  // ? Implement a queue to exit after cutscene?
  check(PlayerBehaviourState != EPlayerState::Cutscene);

  HUD->QuitUIAction();
  if (bInCinematicView) ToggleCinematicView();
}
void APlayerZDCharacter::ResetLocationToSpawnPoint() {
  if (bInCinematicView) ToggleCinematicView();

  TArray<ASpawnPoint*> SpawnPoints = GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass);
  check(SpawnPoints.Num() > 0);
  ASpawnPoint** FoundSpawnPoint = SpawnPoints.FindByPredicate(
      [this](const ASpawnPoint* SpawnPoint) { return SpawnPoint->Level == LevelManager->CurrentLevel; });
  check(FoundSpawnPoint);
  check(*FoundSpawnPoint);

  this->SetActorLocation((*FoundSpawnPoint)->GetActorLocation());
  SpringArmC->SetRelativeLocation(GetActorLocation());
}

// ? Change parameter to ELevel?
void APlayerZDCharacter::EnterNewLevel(ULevelChangeComponent* LevelChangeC) {
  switch (StorePhaseManager->StorePhaseState) {
    case EStorePhaseState::Morning:
    case EStorePhaseState::MorningBuildMode:
      if (LevelManager->CurrentLevel == ELevel::Store && LevelChangeC->LevelToLoad != ELevel::Market) return;
      if (LevelManager->CurrentLevel == ELevel::Market && LevelChangeC->LevelToLoad != ELevel::Store) return;

      if (StorePhaseManager->StorePhaseState == EStorePhaseState::MorningBuildMode)
        StorePhaseManager->ToggleBuildMode();
      break;
    case EStorePhaseState::ShopOpen: return;
    case EStorePhaseState::Night:
      if (LevelManager->CurrentLevel == ELevel::Store && LevelChangeC->LevelToLoad != ELevel::Church) return;
      if (LevelManager->CurrentLevel == ELevel::Church && LevelChangeC->LevelToLoad != ELevel::Store) return;
      break;
  }

  // ? Put in level manager?
  auto LevelReadyFunc = [this, LevelChangeC]() {
    HUD->ShowInGameHudWidget();

    ASpawnPoint* SpawnPoint = *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)
                                   .FindByPredicate([LevelChangeC](const ASpawnPoint* SpawnPoint) {
                                     return SpawnPoint->Level == LevelChangeC->LevelToLoad;
                                   });
    check(SpawnPoint);

    this->SetActorLocation(SpawnPoint->GetActorLocation());
    SpringArmC->SetRelativeLocation(GetActorLocation());
  };
  HUD->HideInGameHudWidget();
  LevelManager->BeginLoadLevel(LevelChangeC->LevelToLoad, LevelReadyFunc);
}
void APlayerZDCharacter::LeaveStore() {
  ELevel LevelToLoad = ELevel::Market;
  switch (StorePhaseManager->StorePhaseState) {
    case EStorePhaseState::Morning:
    case EStorePhaseState::MorningBuildMode: {
      LevelToLoad = ELevel::Market;
      break;
    }
    case EStorePhaseState::ShopOpen: return;
    case EStorePhaseState::Night: LevelToLoad = ELevel::Church; break;
  }

  // ? Put in level manager?
  auto LevelReadyFunc = [this, LevelToLoad]() {
    HUD->ShowInGameHudWidget();

    ASpawnPoint* SpawnPoint = *GetAllActorsOf<ASpawnPoint>(GetWorld(), SpawnPointClass)
                                   .FindByPredicate([LevelToLoad](const ASpawnPoint* SpawnPoint) {
                                     return SpawnPoint->Level == LevelToLoad;
                                   });
    check(SpawnPoint);

    this->SetActorLocation(SpawnPoint->GetActorLocation());
    SpringArmC->SetRelativeLocation(GetActorLocation());
  };
  HUD->HideInGameHudWidget();
  LevelManager->BeginLoadLevel(LevelToLoad, LevelReadyFunc);
}

void APlayerZDCharacter::GameOverReset() {
  ChangePlayerState(EPlayerState::GameOver);
  UGameplayStatics::SetGamePaused(GetWorld(), true);

  if (bInCinematicView) ToggleCinematicView();
  HUD->SetAndOpenGameOverView();
}