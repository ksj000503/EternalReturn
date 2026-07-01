#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "BackendConnectionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBackendMessageReceived, const FString&, MessageType, const FString&, RawJson);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackendConnectionChanged, bool, bConnected);

UCLASS()
class ETERNALRETURN_API UBackendConnectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Backend")
	bool ConnectToBackend(const FString& ServerIP, int32 ServerPort);

	UFUNCTION(BlueprintCallable, Category = "Backend")
	void DisconnectFromBackend();

	UFUNCTION(BlueprintCallable, Category = "Backend")
	bool SendBackendMessage(const FString& MessageType, const TMap<FString, FString>& StringFields);

	UFUNCTION(BlueprintCallable, Category = "Backend")
	bool IsConnectedToBackend() const;

	UPROPERTY(BlueprintAssignable, Category = "Backend")
	FOnBackendMessageReceived OnMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Backend")
	FOnBackendConnectionChanged OnConnectionChanged;

private:

	FSocket* BackendSocket = nullptr;

	FRunnableThread* ReceiveThread = nullptr;

	class FBackendReceiveWorker* ReceiveWorker = nullptr;

	bool SendRawMessage(const FString& Json);

	void HandleReceivedJson(const FString& Json);

	friend class FBackendReceiveWorker;
};

class FBackendReceiveWorker : public FRunnable
{
public:

	FBackendReceiveWorker(UBackendConnectionSubsystem* InOwner, FSocket* InSocket);

	virtual uint32 Run() override;

	virtual void Stop() override;

private:

	TWeakObjectPtr<UBackendConnectionSubsystem> Owner;

	FSocket* Socket;

	bool bStopRequested;
};

