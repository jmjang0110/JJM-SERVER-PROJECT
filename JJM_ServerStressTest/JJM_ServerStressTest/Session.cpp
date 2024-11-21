#include "pch.h"
#include "Session.h"
#include "ExOverlapped.h"

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
	m_ExOver        = new ExOverlapped;
	m_ExOver->Clear();

}

bool Session::DoSend(void* data, const UINT& dataSize)
{
	BYTE* d = reinterpret_cast<BYTE*>(data);
	
	ExOverlapped* over = new ExOverlapped;
	over->Clear();

	over->m_WSABuf.buf = reinterpret_cast<CHAR*>(over->m_Buffer);
	over->m_WSABuf.len = dataSize;
	over->m_IOtype     = IO_TYPE::WRITE;
	DWORD flag = 0;
	memcpy(over->m_Buffer, data, dataSize);

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
	return false;
}

bool Session::OnRecv()
{
	return false;
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
