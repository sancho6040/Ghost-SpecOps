// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/Object.h"
#include "MenuWidget.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

private:
	UPROPERTY(Transient)
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

private:
	int32 MaxSessionSearches;
	FString MatchType;
	FString PathToLobby;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	virtual void OnMultiplayerSessionCreated(const FName SessionName, const bool bWasSuccessful);

	virtual void OnMultiplayerSessionsFound(const TArray<FOnlineSessionSearchResult>& SearchResults, const bool bWasSuccessful);
	virtual void OnMultiplayerSessionJoined(const EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	virtual void OnMultiplayerSessionDestroyed(const bool bWasSuccessful);

	UFUNCTION()
	virtual void OnMultiplayerSessionStarted(const FName SessionName, const bool bWasSuccessful);
	
public:
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm="NewMatchType,PathToLobby"))
	void SetupMenu(const int32 NewMaxSearchSessions = 10000, const FString& NewMatchType = TEXT("DefaultMatchType"), const FString& NewPathToLobby = TEXT("/Game/ThirdPerson/Maps/Lobby"));

private:
	void TearDownMenu();

private:
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

	void EnableButtons();
	void DisableButtons();
};
