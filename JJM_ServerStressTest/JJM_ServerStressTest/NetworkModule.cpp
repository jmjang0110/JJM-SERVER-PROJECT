#include "pch.h"
#include "NetworkModule.h"
#include "Session.h"

std::array<Session, MAX_CLIENT> NetworkModule::m_Sessions;
std::array<int, MAX_CLIENT>		NetworkModule::m_Client_ID_map;

void NetworkModule::Init()
{
	std::wcout.imbue(std::locale("korean"));
	WSADATA wsaData;
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != S_OK)
		assert(0);

	// IOCP 생성하자.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); 
	if (m_hIOCP == NULL)
		assert(0);
}

bool NetworkModule::GQCS(CompletionTask& ct)
{
	DWORD waitTime = INFINITE;
	WSAOVERLAPPED* over{};
	ct.success = ::GetQueuedCompletionStatus(m_hIOCP
										  , &ct.bytes
										  , (PULONG_PTR)&ct.key_ID
										  , &over
										  , waitTime);

	ct.exOver = reinterpret_cast<ExOverlapped*>(over);
	return ct.success;
}

void NetworkModule::Try_Connect_Session_ToServer()
{

	/* Session을 Server에 Connect 시도하자 (딜레이에 따라 조정) */

	m_Last_connected_time = std::chrono::high_resolution_clock::now();

	int ID = m_Connected_clients_num;

	// Connect ! 
	Session& client = m_Sessions[ID];
	client.Init(ID);
	bool ret = client.Connect(SERVER_IP, SERVER_PORT);
	if (ret == false)
		return;
	client.CreateIOCP(m_hIOCP, ID, 0);

}

void NetworkModule::Disconnect_Session_FromServer(int ID)
{

	// Session이 Connect되어있는 상태를 false로 atomic 하게 변경 

	bool status = true;
	if (std::atomic_compare_exchange_strong(&m_Sessions[ID].m_IsConnected, &status, false))
	{

		m_Sessions[ID].Disconnect();
		m_Active_clients_num--;
		
	}
}
