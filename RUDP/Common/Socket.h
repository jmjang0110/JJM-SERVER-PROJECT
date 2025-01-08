#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <string>
#include <sstream>

enum class ProtocolType
{
	NONE,
	TCP,
	UDP,

};

class Socket
{
protected:
	ProtocolType	m_pType = ProtocolType::NONE;
	SOCKET			m_SocketHandle{};
	sockaddr_in		m_HostEndPoint{};

public:
	Socket(ProtocolType type);
	virtual ~Socket();

public:
	bool Connect(std::string& peer, u_short port);
	bool Bind(u_short port, std::string ip = "");
	void Close();

public:
	sockaddr_in StringToEndPoint(std::string& peer);
	std::string GetPeerIPandPort(const sockaddr_in& addr);
	void PrintIPansPort();

public:
	void SetProtocol(ProtocolType type) { m_pType = type; }
	ProtocolType GetProtocol() const { return m_pType; }

	void SetSocketHandle(SOCKET handle) { m_SocketHandle = handle; }
	SOCKET GetSocketHandle() const { return m_SocketHandle; }

	void SetAddress(const sockaddr_in& addr) { m_HostEndPoint = addr; }
	sockaddr_in& GetAddress()  { return m_HostEndPoint; }

	bool IsValid();

};
