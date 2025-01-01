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
	
	// OnSend 에서 메모리 해제 
	ExOverlapped* over = new ExOverlapped;
	over->Clear();
	// 데이터 크기 검증
	if (dataSize + packetheadersize > BUF_SIZE) {
		delete over;
		return false; // 데이터가 너무 큼
	}

	over->m_WSABuf.buf = reinterpret_cast<CHAR*>(over->m_Buffer);
	over->m_WSABuf.len = dataSize + packetheadersize;
	over->m_IOtype     = IO_TYPE::WRITE;
	DWORD flag = 0;

	// 헤더 작성
	PacketHeader header;
	header.PacketSize = over->m_WSABuf.len;  // 전체 패킷 크기
	header.ProtocolID = protocolID;                     // 프로토콜 ID

	memcpy(over->m_Buffer, &header, packetheadersize);	// 헤더 복사
	memcpy(over->m_Buffer + packetheadersize, data, dataSize); // 데이터 복사

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
		return false; // 수신된 데이터가 없으면 처리 중단
	}

	// 남아있는 데이터 크기 갱신
	m_Remain_Recv_DataSize += bytes;

	BYTE* buffer = over->m_Buffer;
	UINT32 ProcessDataSize = 0; // 처리된 데이터 크기
	UINT32 TotalSize = m_Remain_Recv_DataSize;

	while (ProcessDataSize < TotalSize) {
		UINT32 RemainSize = TotalSize - ProcessDataSize;

		// 남은 데이터가 패킷 헤더 크기보다 작으면 다음 수신에서 처리
		if (RemainSize < sizeof(PacketHeader)) {
			break;
		}

		// 패킷 헤더 읽기
		BYTE* startBufferPtr = buffer + ProcessDataSize;
		PacketHeader* packet = reinterpret_cast<PacketHeader*>(startBufferPtr);

		// 패킷 크기 유효성 검사
		if (packet->PacketSize < sizeof(PacketHeader) || packet->PacketSize > 1024) {
			// 비정상적인 패킷 크기 (패킷 손상 가능성)
			printf("Invalid packet size: %u\n", packet->PacketSize);
			return false; // 연결을 종료하거나 에러 처리
		}

		// 남은 데이터가 패킷 전체 크기보다 작으면 다음 수신에서 처리
		if (RemainSize < packet->PacketSize) {
			break;
		}

		// 패킷 처리
		InterpretPacket(startBufferPtr);

		// 처리된 데이터 크기 증가
		ProcessDataSize += packet->PacketSize;
	}

	// 처리되지 않은 데이터는 버퍼 맨 앞으로 이동
	m_Remain_Recv_DataSize = TotalSize - ProcessDataSize;
	if (m_Remain_Recv_DataSize > 0) {
		if (m_Remain_Recv_DataSize >= 1024) {
			assert(0); // 비정상적인 데이터 크기
		}
		memmove(buffer, buffer + ProcessDataSize, m_Remain_Recv_DataSize);
	}

	// 추가적인 데이터 수신 준비
	DoRecv();

	return true;
}

bool Session::OnSend(int bytes, ExOverlapped* over)
{
	// DoSend 당시에 ExOverlapped 메모리를 할당해 send 했다. 
	// 따라서 WSAsend가 완료된 후에 ExOverlapped메모리를 해제해야한다.
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
		// DirectX 벡터 생성
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


	// X-Machina 는 socket을 아이디로 해서 갖고 있음 .
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

