#pragma once

// CLIENT, ROOM 
const static  LONG64 MAX_USER            = 4000;
const static  LONG64 MAX_CLIENT          = MAX_USER * 2;
const static  LONG64 MAX_CLIENT_PER_ROOM = 500; // ROOM당 4명으로 수용
const static  LONG64 MAX_ROOM            = MAX_CLIENT / 4; // 서버 전체 ROOM개수

// SERVER IP , PORT 
//const std::string    SERVER_IP           = "127.0.0.1";
const std::string    SERVER_IP           = "172.30.1.53";
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

	Concurrency::concurrent_queue<LONG64> m_Reconnect_clients_Q;
	std::atomic<LONG64>			m_Connected_clients_num{};
	std::atomic<LONG64>			m_Active_clients_num{};
	std::atomic<LONG64>			m_close_ID{};

	TimeStamp					m_Last_connected_time{};

	std::vector<std::thread>	m_tWorkers{}; // worker thread
	std::thread					m_tTest{};	  // stress test thread
	bool						m_NetworkStart = false;

	long long					m_delay{};

public:
	static std::array<LONG64, MAX_CLIENT>		m_Client_ID_map;
	static std::array<Session, MAX_CLIENT>		m_Sessions;

public:
	NetworkModule() {};
	~NetworkModule();

private:
	// Thread 
	void WorkerThread();
	void TestThread();
	// Get Queued Completion Status Task info
	bool GQCS( /*IN-OUT*/ CompletionTask& completionstatus);
	void Process_CompletionTask(LONG64 id, int bytes, ExOverlapped* over);
	// send/recv with Server 
	void Try_Connect_Session_ToServer();
	void Connect_Session_ToServer(LONG64 ID);
	void Disconnect_Session_FromServer();

public:
	void Init();
	void Execute(int workerThread_num);
	int  GetConnectedClientsNum() { return m_Connected_clients_num.load(); }
	LONG64 GetActiveClientsNum() { return m_Active_clients_num.load(); }


	void Exit();
	void PrintErrorDescription(int errorCode);
	void Draw_Sessions();

	int GetClientsInRoom(int roomid, int maxclientsNum);


public:
	// Get 
	long long GetDelay() { return m_delay; }
	void UpDelay() { m_delay++; }
	void DownDelay() { m_delay--; }

	void UpActiveClients() { m_Active_clients_num++; }
};

