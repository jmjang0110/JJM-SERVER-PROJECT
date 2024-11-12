#pragma once

// CLIENT, ROOM 
const static  UINT64 MAX_USER            = 2000;
const static  UINT64 MAX_CLIENT          = MAX_USER * 2;
const static  UINT64 MAX_CLIENT_PER_ROOM = 4; // ROOM�� 4������ ����
const static  UINT64 MAX_ROOM            = MAX_CLIENT / 4; // ���� ��ü ROOM����

// SERVER IP , PORT 
const std::string    SERVER_IP           = "127.0.0.1";
const short          SERVER_PORT         = 7777;

// DELAY 
constexpr int CONNECT_DELAY				 = 50; // ���� �����ð� : 50ms
constexpr int LIMIT_DELAY				 = 1000; // ���� �⤿��   : 1000ms

class ExOverlapped;
// GetQueuedCompletionStatus �ϰ�ó�� ���� ���� 
struct CompletionTask
{
	LONG64			key_ID{};
	DWORD			bytes{};
	BOOL			success{};
	ExOverlapped*	exOver{};
};

class Session;
class NetworkModule
{
private:
	HANDLE						m_hIOCP{};

	std::atomic<int>			m_Connected_clients_num{};
	std::atomic<int>			m_Active_clients_num{};
	TimePoint					m_Last_connected_time{};

	std::vector<std::thread*>	m_tWorkers{};
	std::thread					m_tTest{};

public:
	static std::array<int, MAX_CLIENT>		m_Client_ID_map;
	static std::array<Session, MAX_CLIENT>	m_Sessions;

public:
	void Init();

	// Get Queued Completion Status Task info
	bool GQCS(CompletionTask& completionstatus);

	void Try_Connect_Session_ToServer();
	void Disconnect_Session_FromServer(int ID);


};

