#pragma once

enum class SESSION_STATE
{
	FREE, ACCEPT, IN_GAME,
};

class ExOverlapped;
class Session
{
private:
	int					m_ID = -1;
	SOCKET				m_Socket{};
	ExOverlapped*		m_ExOver{};				   // Recv
	int					m_Remain_Recv_DataSize{};  // 남아있는 데이터 사이즈
	std::mutex			m_SessionLock{};	

public:
	std::atomic<bool>	m_IsConnected{};
	SESSION_STATE		m_State = SESSION_STATE::FREE;
	TimePoint           m_MoveTime{}; // 시간 간격을 두고 move packet 전송

public:
	void Init(int ID);

	// WSA 
	bool DoSend(void* data, const UINT& dataSize);
	bool DoRecv();

	// GQCS
	bool OnRecv();

	bool Connect(const std::string& ip, const short port);
	void Disconnect();
	HANDLE CreateIOCP(HANDLE& iocpsock, ULONG_PTR key, DWORD numofconcurrentThreads);


};

