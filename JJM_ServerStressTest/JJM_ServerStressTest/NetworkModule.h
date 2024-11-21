#pragma once

// CLIENT, ROOM 
const static  UINT64 MAX_USER            = 2000;
const static  UINT64 MAX_CLIENT          = MAX_USER * 2;
const static  UINT64 MAX_CLIENT_PER_ROOM = 4; // ROOM당 4명으로 수용
const static  UINT64 MAX_ROOM            = MAX_CLIENT / 4; // 서버 전체 ROOM개수

// SERVER IP , PORT 
const std::string    SERVER_IP           = "127.0.0.1";
const short          SERVER_PORT         = 7777;

// DELAY 
constexpr int CONNECT_DELAY				 = 50; // 접속 지연시간 : 50ms
constexpr int LIMIT_DELAY				 = 1000; // 렉의 기ㅏ준   : 1000ms

class ExOverlapped;
// GetQueuedCompletionStatus 일감처리 정보 저장 
struct CompletionTask
{
	LONG64			key_ID{};
	DWORD			bytes{};
	BOOL			success{};
	ExOverlapped*	exOver{};
};

class Session;
class NetworkModule : public Singleton<NetworkModule>
{
private:
	FILE*						mConsole{};
private:
	HANDLE						m_hIOCP{};

	std::atomic<LONG64>			m_Connected_clients_num{};
	std::atomic<LONG64>			m_Active_clients_num{};
	std::atomic<LONG64>			m_close_ID{};

	TimeStamp					m_Last_connected_time{};

	std::vector<std::thread>	m_tWorkers{}; // worker thread
	std::thread					m_tTest{};	  // stress test thread
	bool						m_NetworkStart = false;

	int							m_delay{};

public:
	static std::array<LONG64, MAX_CLIENT>		m_Client_ID_map;
	static std::array<Session, MAX_CLIENT>	m_Sessions;

public:
	NetworkModule() {};
	~NetworkModule();

private:
	void WorkerThread();
	void TestThread();
	// Get Queued Completion Status Task info
	bool GQCS( /*IN-OUT*/ CompletionTask& completionstatus);
	void Try_Connect_Session_ToServer();
	void Connect_Session_ToServer(LONG64 ID);
	void Disconnect_Session_FromServer(LONG64 ID);

public:
	void Init();
	void Execute(int workerThread_num);
	int  GetConnectedClientsNum() { return m_Connected_clients_num.load(); }

	void Exit();
	void PrintErrorDescription(int errorCode);

};

