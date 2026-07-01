#include "BackendConnectionSubsystem.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

static void WriteUInt32BigEndian(uint32 Value, uint8* OutBytes)
{
	OutBytes[0] = static_cast<uint8>((Value >> 24) & 0xFF);

	OutBytes[1] = static_cast<uint8>((Value >> 16) & 0xFF);

	OutBytes[2] = static_cast<uint8>((Value >> 8) & 0xFF);

	OutBytes[3] = static_cast<uint8>(Value & 0xFF);
}

static uint32 ReadUInt32BigEndian(const uint8* Bytes)
{
	uint32 Result = (static_cast<uint32>(Bytes[0]) << 24) |
		(static_cast<uint32>(Bytes[1]) << 16) |
		(static_cast<uint32>(Bytes[2]) << 8) |
		(static_cast<uint32>(Bytes[3]));

	return Result;
}

static bool RecvExact(FSocket* Socket, uint8* Buffer, int32 Length)
{
	int32 TotalReceived = 0;

	while (TotalReceived < Length)
	{
		int32 BytesRead = 0;

		bool bSuccess = Socket->Recv(Buffer + TotalReceived, Length - TotalReceived, BytesRead, ESocketReceiveFlags::None);

		if (!bSuccess || BytesRead <= 0)
		{
			return false;
		}

		TotalReceived += BytesRead;
	}

	return true;
}

static bool SendExact(FSocket* Socket, const uint8* Buffer, int32 Length)
{
	int32 TotalSent = 0;

	while (TotalSent < Length)
	{
		int32 BytesSent = 0;

		bool bSuccess = Socket->Send(Buffer + TotalSent, Length - TotalSent, BytesSent);

		if (!bSuccess || BytesSent <= 0)
		{
			return false;
		}

		TotalSent += BytesSent;
	}

	return true;
}

void UBackendConnectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBackendConnectionSubsystem::Deinitialize()
{
	DisconnectFromBackend();

	Super::Deinitialize();
}

bool UBackendConnectionSubsystem::ConnectToBackend(const FString& ServerIP, int32 ServerPort)
{
	if (BackendSocket != nullptr)
	{
		DisconnectFromBackend();
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (!SocketSubsystem)
	{
		return false;
	}

	BackendSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("BackendTCPClient"), false);

	if (!BackendSocket)
	{
		return false;
	}

	FIPv4Address IPAddr;

	bool bParsed = FIPv4Address::Parse(ServerIP, IPAddr);

	if (!bParsed)
	{
		UE_LOG(LogTemp, Error, TEXT("[Backend] 잘못된 IP 주소: %s"), *ServerIP);

		return false;
	}

	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();

	Addr->SetIp(IPAddr.Value);

	Addr->SetPort(ServerPort);

	bool bConnected = BackendSocket->Connect(*Addr);

	if (!bConnected)
	{
		UE_LOG(LogTemp, Error, TEXT("[Backend] 연결 실패: %s:%d"), *ServerIP, ServerPort);

		SocketSubsystem->DestroySocket(BackendSocket);

		BackendSocket = nullptr;

		return false;
	}

	BackendSocket->SetNonBlocking(false);

	ReceiveWorker = new FBackendReceiveWorker(this, BackendSocket);

	ReceiveThread = FRunnableThread::Create(ReceiveWorker, TEXT("BackendReceiveThread"));

	UE_LOG(LogTemp, Log, TEXT("[Backend] 연결 성공: %s:%d"), *ServerIP, ServerPort);

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnConnectionChanged.Broadcast(true);
		});

	return true;
}

void UBackendConnectionSubsystem::DisconnectFromBackend()
{
	if (ReceiveThread)
	{
		if (ReceiveWorker)
		{
			ReceiveWorker->Stop();
		}

		ReceiveThread->WaitForCompletion();

		delete ReceiveThread;

		ReceiveThread = nullptr;
	}

	if (ReceiveWorker)
	{
		delete ReceiveWorker;

		ReceiveWorker = nullptr;
	}

	if (BackendSocket)
	{
		BackendSocket->Close();

		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(BackendSocket);

		BackendSocket = nullptr;
	}
}

bool UBackendConnectionSubsystem::IsConnectedToBackend() const
{
	bool bIsConnected = BackendSocket != nullptr && BackendSocket->GetConnectionState() == SCS_Connected;

	return bIsConnected;
}

bool UBackendConnectionSubsystem::SendBackendMessage(const FString& MessageType, const TMap<FString, FString>& StringFields)
{
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetStringField(TEXT("type"), MessageType);

	for (const auto& Pair : StringFields)
	{
		JsonObject->SetStringField(Pair.Key, Pair.Value);
	}

	FString OutputString;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(JsonObject, Writer);

	bool bSent = SendRawMessage(OutputString);

	return bSent;
}

bool UBackendConnectionSubsystem::SendRawMessage(const FString& Json)
{
	if (!BackendSocket)
	{
		return false;
	}

	FTCHARToUTF8 Utf8Json(*Json);

	const uint8* BodyBytes = reinterpret_cast<const uint8*>(Utf8Json.Get());

	int32 BodyLength = Utf8Json.Length();

	uint8 LengthPrefix[4];

	WriteUInt32BigEndian(static_cast<uint32>(BodyLength), LengthPrefix);

	bool bLengthSent = SendExact(BackendSocket, LengthPrefix, 4);

	if (!bLengthSent)
	{
		return false;
	}

	bool bBodySent = SendExact(BackendSocket, BodyBytes, BodyLength);

	return bBodySent;
}

void UBackendConnectionSubsystem::HandleReceivedJson(const FString& Json)
{
	TSharedPtr<FJsonObject> JsonObject;

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

	FString MessageType;

	bool bParsed = FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid();

	if (bParsed)
	{
		JsonObject->TryGetStringField(TEXT("type"), MessageType);
	}

	OnMessageReceived.Broadcast(MessageType, Json);
}

FBackendReceiveWorker::FBackendReceiveWorker(UBackendConnectionSubsystem* InOwner, FSocket* InSocket)
	: Owner(InOwner), Socket(InSocket), bStopRequested(false)
{
}

uint32 FBackendReceiveWorker::Run()
{
	while (!bStopRequested)
	{
		uint8 LengthPrefix[4];

		bool bLengthReceived = RecvExact(Socket, LengthPrefix, 4);

		if (!bLengthReceived)
		{
			break;
		}

		uint32 BodyLength = ReadUInt32BigEndian(LengthPrefix);

		constexpr uint32 MaxMessageSize = 1024 * 1024;

		if (BodyLength == 0 || BodyLength > MaxMessageSize)
		{
			break;
		}

		TArray<uint8> Buffer;

		Buffer.SetNumUninitialized(BodyLength + 1);

		Buffer[BodyLength] = 0;

		bool bBodyReceived = RecvExact(Socket, Buffer.GetData(), BodyLength);

		if (!bBodyReceived)
		{
			break;
		}

		FString Json(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Buffer.GetData())));

		TWeakObjectPtr<UBackendConnectionSubsystem> WeakOwner = Owner;

		AsyncTask(ENamedThreads::GameThread, [WeakOwner, Json]()
			{
				if (WeakOwner.IsValid())
				{
					WeakOwner->HandleReceivedJson(Json);
				}
			});
	}

	return 0;
}

void FBackendReceiveWorker::Stop()
{
	bStopRequested = true;

	if (Socket)
	{
		Socket->Close();
	}
}