#include "pch.h"
#include "Session.h"
#include "ExOverlapped.h"
#include "NetworkModule.h"

#undef max
#include <flatbuffers/flatbuffers.h>
#include "xmachina_protocol/FBProtocol_generated.h"
#include "xmachina_protocol/Enum_generated.h"
#include "xmachina_protocol/Struct_generated.h"
#include "xmachina_protocol/Transform_generated.h"

Session::Session()
{
}

Session::~Session()
{
	if (m_ExOver)
	{
		delete m_ExOver;
		m_ExOver = nullptr;
	}
}

void Session::Init(int ID)
{
	m_ID			= ID;
	m_IsConnected	= false;
	m_Socket        = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	
	if (m_Socket == INVALID_SOCKET) {
		std::cout << "INVALID SCOKET ( Session::Init(" << m_ID << ")\n";
		assert(0);
	}
	
	if (!m_ExOver) {
		m_ExOver        = new ExOverlapped;
		m_ExOver->Clear();
	}
}

bool Session::DoSend(void* data, const UINT& dataSize, UINT16 protocolID)
{
	int packetheadersize = sizeof(PacketHeader);
	BYTE* d = reinterpret_cast<BYTE*>(data);
	
	// OnSend ���� �޸� ���� 
	ExOverlapped* over = new ExOverlapped;
	over->Clear();
	// ������ ũ�� ����
	if (dataSize + packetheadersize > BUF_SIZE) {
		delete over;
		return false; // �����Ͱ� �ʹ� ŭ
	}

	over->m_WSABuf.buf = reinterpret_cast<CHAR*>(over->m_Buffer);
	over->m_WSABuf.len = dataSize + packetheadersize;
	over->m_IOtype     = IO_TYPE::WRITE;
	DWORD flag = 0;

	// ��� �ۼ�
	PacketHeader header;
	header.PacketSize = over->m_WSABuf.len;  // ��ü ��Ŷ ũ��
	header.ProtocolID = protocolID;                     // �������� ID

	memcpy(over->m_Buffer, &header, packetheadersize);	// ��� ����
	memcpy(over->m_Buffer + packetheadersize, data, dataSize); // ������ ����

	void* p = &over->m_Over;

	//m_SessionLock.lock();
	int result = ::WSASend(m_Socket, &over->m_WSABuf, 1, NULL, 0, &over->m_Over, NULL);
	if (SOCKET_ERROR == result) {
		int error = ::WSAGetLastError();
		//if (error != WSA_IO_PENDING) {
		//	delete over;
			//m_SessionLock.unlock();
			return false;
		//}
	}

	//m_SessionLock.unlock();
	return true;
}

bool Session::DoRecv()
{
	m_ExOver->m_IOtype = IO_TYPE::READ;
	memset(&m_ExOver->m_Over, 0, sizeof(m_ExOver->m_Over));
	m_ExOver->m_WSABuf.buf = reinterpret_cast<char*>(m_ExOver->m_Buffer + m_Remain_Recv_DataSize);
	m_ExOver->m_WSABuf.len = BUF_SIZE - m_Remain_Recv_DataSize;

	DWORD flag = 0;
	int result = ::WSARecv(m_Socket, &m_ExOver->m_WSABuf, 1, 0, &flag, &m_ExOver->m_Over, NULL);
	if (SOCKET_ERROR == result) {
		int error = ::WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			return false;
		}
	}

	return true;
}

bool Session::OnRecv(int bytes, ExOverlapped* over)
{
	if (bytes <= 0) {
		return false; // ���ŵ� �����Ͱ� ������ ó�� �ߴ�
	}

	// �����ִ� ������ ũ�� ����
	m_Remain_Recv_DataSize += bytes;

	BYTE* buffer = over->m_Buffer;
	UINT32 ProcessDataSize = 0; // ó���� ������ ũ��
	UINT32 TotalSize = m_Remain_Recv_DataSize;

	while (ProcessDataSize < TotalSize) {
		UINT32 RemainSize = TotalSize - ProcessDataSize;

		// ���� �����Ͱ� ��Ŷ ��� ũ�⺸�� ������ ���� ���ſ��� ó��
		if (RemainSize < sizeof(PacketHeader)) {
			break;
		}

		// ��Ŷ ��� �б�
		BYTE* startBufferPtr = buffer + ProcessDataSize;
		PacketHeader* packet = reinterpret_cast<PacketHeader*>(startBufferPtr);

		// ��Ŷ ũ�� ��ȿ�� �˻�
		if (packet->PacketSize < sizeof(PacketHeader) || packet->PacketSize > 1024) {
			// ���������� ��Ŷ ũ�� (��Ŷ �ջ� ���ɼ�)
			printf("Invalid packet size: %u\n", packet->PacketSize);
			return false; // ������ �����ϰų� ���� ó��
		}

		// ���� �����Ͱ� ��Ŷ ��ü ũ�⺸�� ������ ���� ���ſ��� ó��
		if (RemainSize < packet->PacketSize) {
			break;
		}

		// ��Ŷ ó��
		InterpretPacket(startBufferPtr);

		// ó���� ������ ũ�� ����
		ProcessDataSize += packet->PacketSize;
	}

	// ó������ ���� �����ʹ� ���� �� ������ �̵�
	m_Remain_Recv_DataSize = TotalSize - ProcessDataSize;
	if (m_Remain_Recv_DataSize > 0) {
		if (m_Remain_Recv_DataSize >= 1024) {
			assert(0); // ���������� ������ ũ��
		}
		memmove(buffer, buffer + ProcessDataSize, m_Remain_Recv_DataSize);
	}

	// �߰����� ������ ���� �غ�
	DoRecv();

	return true;
}

bool Session::OnSend(int bytes, ExOverlapped* over)
{
	// DoSend ��ÿ� ExOverlapped �޸𸮸� �Ҵ��� send �ߴ�. 
	// ���� WSAsend�� �Ϸ�� �Ŀ� ExOverlapped�޸𸮸� �����ؾ��Ѵ�.
	if (bytes != over->m_WSABuf.len) {
		delete over;
		over = nullptr;
		return false;
	}

	delete over;
	over = nullptr;
	return true;

}


bool Session::Connect(const std::string& ip, const short port)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	int ret = ::WSAConnect(m_Socket
						 , reinterpret_cast<const sockaddr*>(&sockAddr)
						 , sizeof(sockaddr), NULL, NULL, NULL, NULL);

	return ret == S_OK;
}

void Session::Disconnect()
{
	::closesocket(m_Socket);
}

HANDLE Session::CreateIOCP(HANDLE& iocpsock, ULONG_PTR key, DWORD numofconcurrentThreads)
{
	return ::CreateIoCompletionPort((HANDLE)m_Socket, iocpsock, key, numofconcurrentThreads);
}

bool Session::Send_CPkt_LogIn(std::string id, std::string password)
{

	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_LogIn(builder, builder.CreateString(id), builder.CreateString(password));
	builder.Finish(cpkt);
	return DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_LogIn);

}

bool Session::Recv_SPkt_LogIn()
{
	return true;

}

bool Session::Send_CPkt_EnterLobby()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_EnterLobby(builder, m_ID);
	builder.Finish(cpkt);
	return DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_EnterLobby);
}

bool Session::Recv_SPkt_EnterLobby()
{
	return true;

}

bool Session::Send_CPkt_PlayGame()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_PlayGame(builder);
	builder.Finish(cpkt);
	return DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_PlayGame);

}

bool Session::Recv_SPkt_PlayGame()
{
	return true;

}

bool Session::Send_CPkt_EnterGame()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_EnterGame(builder, m_ID);
	builder.Finish(cpkt);
	return DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_EnterGame);

}

bool Session::Send_CPkt_Transform()
{
	m_MoveDir = Vec3{ (float)(rand() % 10), 0.f, (float)(rand() % 10) };
	if (m_MoveDirTime >= 3.f) {
		m_MoveDirTime = 0.f;
		// DirectX ���� ����
		DirectX::XMVECTOR moveDirVector = DirectX::XMVectorSet(m_MoveDir.x, m_MoveDir.y, m_MoveDir.z, 0.0f);
		moveDirVector = DirectX::XMVector3Normalize(moveDirVector);
		DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&m_MoveDir), moveDirVector);
	};

	flatbuffers::FlatBufferBuilder builder{};

	m_Pos.x = rand() % 1000;
	m_Pos.z = rand() % 1000;
	auto dir = FBProtocol::CreateVector3(builder, m_MoveDir.x, m_MoveDir.y, m_MoveDir.z);
	auto pos = FBProtocol::CreateVector3(builder, m_Pos.x, m_Pos.y, m_Pos.z);
	auto rot = FBProtocol::CreateVector3(builder, 0.f, 0.f, 0.f);
	auto trans = FBProtocol::CreateTransform(builder, pos, rot);
	auto spine = FBProtocol::CreateVector3(builder, 0.f, 0.f, 0.f);

	auto speed = 2.f;
	auto latency = 2.f;
	auto move_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	auto pkt = FBProtocol::CreateCPkt_Player_Transform(builder,
		m_ID, FBProtocol::PLAYER_MOTION_STATE_TYPE::PLAYER_MOTION_STATE_TYPE_RUN,
		latency, speed, dir, trans, spine, 0.f, 0.f, move_time);

	builder.Finish(pkt);


	//std::cout << "Send Transform Player Packet \n";
	return DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_Player_Transform);
}

bool Session::Recv_SPkt_Transform(const void* data)
{
	const auto* packet = flatbuffers::GetRoot<FBProtocol::SPkt_Player_Transform>(data);
	if (!packet)
		assert(0);

	//std::cout << "[" << m_ID << "] - Latency : " << packet->latency() << "\n";

	long long prevtime = packet->move_time();
	int client_id = packet->client_id();


	// X-Machina �� socket�� ���̵�� �ؼ� ���� ���� .
	if (0 != prevtime && client_id == m_ID) {
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count() - prevtime;

		auto curr_delay = NetworkModule::GetInstance()->GetDelay();
		if (curr_delay < dt) {
			//std::cout << "delay : " << curr_delay << "\n";
			NetworkModule::GetInstance()->UpDelay();
		}
		else if (curr_delay > dt) {
			//std::cout << "delay : " << curr_delay << "\n";
			NetworkModule::GetInstance()->DownDelay();
		}

	}
	//else
		//assert(0);

	return false;
}

void Session::InterpretPacket(BYTE* packet)
{
	const uint8_t* pData = (const uint8_t*)packet;
	PacketHeader* Head = reinterpret_cast<PacketHeader*>(packet);
	const void* DataPtr = packet + sizeof(PacketHeader);

	switch (Head->ProtocolID)
	{
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_LogIn: {
		//std::cout << "FBsProtocolID_SPkt_LogIn Recv \n";
		Send_CPkt_EnterLobby();
		m_IsConnected.store(true);
		NetworkModule::GetInstance()->UpActiveClients();

	}
		break;
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_EnterLobby: {
		//std::cout << "FBsProtocolID_SPkt_EnterLobby Recv \n";
	}
		break;
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_PlayGame: {
		//std::cout << "FBsProtocolID_SPkt_PlayGame Recv \n";
		Send_CPkt_EnterGame();
	}
		break;
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_EnterGame: {
		//std::cout << "FBsProtocolID_CPkt_EnterGame Recv \n";

	}
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_NewPlayer: {
		//std::cout << "FBsProtocolID_SPkt_NewPlayer Recv \n";
	}
		break;
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_Player_Transform: {
		//std::cout << "[" << m_ID << "] FBsProtocolID_SPkt_Player_Transform Recv \n";
		Recv_SPkt_Transform(DataPtr);
	}
		break;

	default:
		break;
	}

}

