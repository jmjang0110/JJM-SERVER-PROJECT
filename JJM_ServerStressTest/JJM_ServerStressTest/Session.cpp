#include "pch.h"
#include "Session.h"
#include "ExOverlapped.h"

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
	if (!m_ExOver) {
		m_ExOver        = new ExOverlapped;
		m_ExOver->Clear();
	}
}

bool Session::DoSend(void* data, const UINT& dataSize, UINT16 protocolID)
{
	int packetheadersize = sizeof(PacketHeader);
	BYTE* d = reinterpret_cast<BYTE*>(data);
	
	ExOverlapped* over = new ExOverlapped;
	over->Clear();

	over->m_WSABuf.buf = reinterpret_cast<CHAR*>(over->m_Buffer);
	over->m_WSABuf.len = dataSize + packetheadersize;
	over->m_IOtype     = IO_TYPE::WRITE;
	DWORD flag = 0;

	// ��� �ۼ�
	PacketHeader header;
	header.PacketSize = over->m_WSABuf.len;  // ��ü ��Ŷ ũ��
	header.ProtocolID = protocolID;                     // �������� ID

	// ��� ����
	memcpy(over->m_Buffer, &header, packetheadersize);

	// ������ ����
	memcpy(over->m_Buffer + packetheadersize, data, dataSize);

	int result = ::WSASend(m_Socket, &over->m_WSABuf, 1, NULL, 0, &over->m_Over, NULL);
	if (SOCKET_ERROR == result) {
		int error = ::WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			delete over;
			return false;
		}
	}

	return true;
}

bool Session::DoRecv()
{
	m_ExOver->m_IOtype = IO_TYPE::READ;
	memset(&m_ExOver->m_Over, 0, sizeof(m_ExOver->m_Over));
	m_ExOver->m_WSABuf.buf = reinterpret_cast<char*>(m_ExOver->m_Buffer + m_Remain_Recv_DataSize);
	m_ExOver->m_WSABuf.len = BUF_SIZE + m_Remain_Recv_DataSize;

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
	// ���ŵ� �������� ũ�⸦ Ȯ���մϴ�.
	if (bytes <= 0) {
		return false; // �����Ͱ� ������ ó�� �ߴ�
	}

	// �����ִ� ������ ũ�⸦ ����
	m_Remain_Recv_DataSize += bytes;

	// ���� ó���ؾ� �� ������
	BYTE* buffer = over->m_Buffer;
	UINT32 ProcessDataSize = 0;
	UINT32 TotalSize = bytes;

	while (ProcessDataSize < TotalSize) {
		UINT32 RemainSize = TotalSize - ProcessDataSize;

		// ���� �����Ͱ� ��Ŷ ��� ũ�⺸�� ������ ���� ���ſ��� ó��
		if (RemainSize < sizeof(PacketHeader)) {
			break;
		}

		// ��Ŷ ����� ����
		BYTE* startBufferPtr = buffer + ProcessDataSize;
		PacketHeader* packet = reinterpret_cast<PacketHeader*>(startBufferPtr);

		// ���� �����Ͱ� ��Ŷ ��ü ũ�⺸�� ������ ���� ���ſ��� ó��
		if (RemainSize < packet->PacketSize) {
			break;
		}

		InterpretPacket(startBufferPtr);

		// ó���� ������ ũ�⸸ŭ ����
		ProcessDataSize += packet->PacketSize;
	}

	// ó������ ���� �����ʹ� ���� �� ������ �̵�
	m_Remain_Recv_DataSize = TotalSize - ProcessDataSize;
	// �߰����� ������ ���� �غ�
	DoRecv();

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

void Session::Send_CPkt_LogIn(std::string id, std::string password)
{

	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_LogIn(builder, builder.CreateString(id), builder.CreateString(password));
	builder.Finish(cpkt);
	DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_LogIn);

}

void Session::Recv_SPkt_LogIn()
{
}

void Session::Send_CPkt_EnterLobby()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_EnterLobby(builder, m_ID);
	builder.Finish(cpkt);
	DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_EnterLobby);
}

void Session::Recv_SPkt_EnterLobby()
{
}

void Session::Send_CPkt_PlayGame()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_PlayGame(builder);
	builder.Finish(cpkt);
	DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_PlayGame);

}

void Session::Recv_SPkt_PlayGame()
{
}

void Session::Send_CPkt_EnterGame()
{
	flatbuffers::FlatBufferBuilder builder;
	auto cpkt = FBProtocol::CreateCPkt_EnterGame(builder, m_ID);
	builder.Finish(cpkt);
	DoSend(builder.GetBufferPointer(), builder.GetSize(), FBProtocol::FBsProtocolID_CPkt_EnterGame);

}

void Session::InterpretPacket(BYTE* packet)
{
	std::cout << "InterpretPacket \n";

	const uint8_t* pData = (const uint8_t*)packet;
	PacketHeader* Head = reinterpret_cast<PacketHeader*>(packet);
	const void* DataPtr = packet + sizeof(PacketHeader);

	switch (Head->ProtocolID)
	{
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_LogIn: {
		std::cout << "FBsProtocolID_SPkt_LogIn Recv \n";
		Send_CPkt_EnterLobby();
		break;
	}
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_EnterLobby: {
		std::cout << "FBsProtocolID_SPkt_EnterLobby Recv \n";

		break;
	}
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_PlayGame: {
		std::cout << "FBsProtocolID_SPkt_PlayGame Recv \n";
		Send_CPkt_EnterGame();
		break;
	}
	case FBProtocol::FBsProtocolID::FBsProtocolID_SPkt_NewPlayer: {
		std::cout << "FBsProtocolID_SPkt_NewPlayer Recv \n";
		break;
	}
	default:
		break;
	}

}

