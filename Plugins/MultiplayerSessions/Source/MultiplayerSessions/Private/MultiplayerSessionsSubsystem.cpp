// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}

	BindDelegates();
}

void UMultiplayerSessionsSubsystem::BindDelegates()
{
	if (OnlineSessionInterface)
	{
		const auto OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
		OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		const auto OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
		OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

		const auto OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
		OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

		const auto OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

		const auto OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnStartSessionComplete);
		OnlineSessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	}
}

void UMultiplayerSessionsSubsystem::RequestCreateSession(const int32 NumPublicConnections, const FString& MatchType)
{
	if (!OnlineSessionInterface.IsValid() || !IOnlineSubsystem::Get())
	{
		return;
	}

	if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateSessionOnDestroy = true;
		LastCreateRequestPublicConnections = NumPublicConnections;
		LastCreateRequestMatchType = MatchType;
		
		DestroySession();
		return;
	}

	SessionSettings = {};
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings.NumPublicConnections = NumPublicConnections;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.BuildUniqueId = 1;
	SessionSettings.Set(TEXT("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		const bool bIsSuccessfulRequest = OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
		if (!bIsSuccessfulRequest)
		{
			OnCreateSessionComplete(NAME_GameSession, false);
		}
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.f, FColor::Cyan, FString::Printf(TEXT("Created session: %s"), *SessionName.ToString()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.f, FColor::Red, FString("Failed to create session!"));
	}

	OnMultiplayerSessionCreatedDelegate.Broadcast(SessionName, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		OnMultiplayerSessionDestroyed.Broadcast(false);
		return;
	}

	if (!OnlineSessionInterface->DestroySession(NAME_GameSession))
	{
		OnMultiplayerSessionDestroyed.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		RequestCreateSession(LastCreateRequestPublicConnections, LastCreateRequestMatchType);
	}

	OnMultiplayerSessionDestroyed.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::FindSessions(const int32 MaxSearchResults)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 90.f, FColor::Cyan, TEXT("FindSessions Called!!"));

	if (!OnlineSessionInterface.IsValid() || !IOnlineSubsystem::Get())
	{
		return;
	}

	SessionSearchResults = MakeShareable(new FOnlineSessionSearch());
	SessionSearchResults->MaxSearchResults = MaxSearchResults;
	SessionSearchResults->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearchResults->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		const bool bIsSuccessfulRequest = OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearchResults.ToSharedRef());
		if (!bIsSuccessfulRequest)
		{
			OnFindSessionsComplete(false);
		}
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(const bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 90.f, FColor::Cyan, TEXT("OnFindSessionsComplete Called!!"));

	const bool bValidResults = bWasSuccessful && SessionSearchResults->SearchResults.Num() > 0;
	OnMultiplayerFindSessionsComplete.Broadcast(SessionSearchResults->SearchResults, bValidResults);
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionSearchResult)
{
	if (!OnlineSessionInterface)
	{
		OnJoinSessionComplete(NAME_GameSession, EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		const bool bIsSuccessfulRequest = OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearchResult);
		if (!bIsSuccessfulRequest)
		{
			OnJoinSessionComplete(NAME_GameSession, EOnJoinSessionCompleteResult::UnknownError);
		}
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(const FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	OnMultiplayerJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
}

UMultiplayerSessionsSubsystem* UMultiplayerSessionsSubsystem::Get(const UGameInstance* GameInstance)
{
	return GameInstance ? GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr;
}
