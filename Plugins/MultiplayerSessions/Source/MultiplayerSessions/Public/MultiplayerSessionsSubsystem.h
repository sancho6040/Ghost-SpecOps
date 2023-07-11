// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerSessionCreated, const FName, SessionName, const bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SearchResults, const bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerJoinSessionComplete, const EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionDestroyed, const bool, bWassuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerStartSessionComplete, const FName, SessionName, const bool, bWassuccessful);

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	IOnlineSessionPtr OnlineSessionInterface;
	
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearchResults;

private:
	bool bCreateSessionOnDestroy = false;
	int32 LastCreateRequestPublicConnections;
	FString LastCreateRequestMatchType;

public:
	FOnMultiplayerSessionCreated OnMultiplayerSessionCreatedDelegate;
	FOnMultiplayerFindSessionsComplete OnMultiplayerFindSessionsComplete;

	FOnMultiplayerJoinSessionComplete OnMultiplayerJoinSessionComplete;
	FOnMultiplayerSessionDestroyed OnMultiplayerSessionDestroyed;

	FOnMultiplayerStartSessionComplete OnMultiplayerSessionStarted;
	
public:
	UMultiplayerSessionsSubsystem();
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void BindDelegates();

public:
	void RequestCreateSession(const int32 NumPublicConnections, const FString& MatchType);
	void OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful);
	
	void DestroySession();
	void OnDestroySessionComplete(const FName SessionName, const bool bWasSuccessful);
	
	void FindSessions(int32 MaxSearchResults);
	void OnFindSessionsComplete(const bool bWasSuccessful);
	
	void JoinSession(const FOnlineSessionSearchResult& SessionSearchResult);
	void OnJoinSessionComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void StartSession();
	void OnStartSessionComplete(const FName SessionName, const bool bWasSuccessful);

public:
	UFUNCTION(BlueprintPure)
	static UMultiplayerSessionsSubsystem* Get(const UGameInstance* GameInstance);

	FORCEINLINE IOnlineSessionPtr GetOnlineSubsystemInterface() const { return OnlineSessionInterface; }
};
