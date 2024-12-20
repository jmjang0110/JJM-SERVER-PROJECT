#pragma once

enum class SESSION_STATE
{
	FREE, ACCEPT, IN_GAME,
};

struct PacketHeader
{
	UINT16 PacketSize = 0;
	UINT16 ProtocolID = 0;
};

struct Vec3 {
	float x = 0, y = 0, z = 0;
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
	TimeStamp           m_MoveTime{}; // 시간 간격을 두고 move packet 전송

public:
	Vec3				m_Pos{};
	Vec3				m_MoveDir{};
	float				m_MoveDirTime = 0; // 일정 시간동안 해당 방향으로 이동 


public:
	Session();
	~Session();

	void Init(int ID);

	// WSA 
	bool DoSend(void* data, const UINT& dataSize, UINT16 protocolID);
	bool DoRecv();

	// GQCS
	bool OnRecv(int bytes, ExOverlapped* over);
	bool OnSend(int bytes, ExOverlapped* over);



	bool Connect(const std::string& ip, const short port);
	void Disconnect();
	HANDLE CreateIOCP(HANDLE& iocpsock, ULONG_PTR key, DWORD numofconcurrentThreads);

	bool Send_CPkt_LogIn(std::string id, std::string password);
	bool Recv_SPkt_LogIn();

	bool Send_CPkt_EnterLobby();
	bool Recv_SPkt_EnterLobby();

	bool Send_CPkt_PlayGame();
	bool Recv_SPkt_PlayGame();

	bool Send_CPkt_EnterGame();

	bool Send_CPkt_Transform();
	bool Recv_SPkt_Transform(const void* data);


	void InterpretPacket(BYTE* packet);
};

