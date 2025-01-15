#include "Socket.h"
#include <iostream>
#include <fcntl.h>

Socket::Socket(ProtocolType type)
{
	m_pType = type;
}

Socket::~Socket()
{
}


bool Socket::Connect(std::string& peer, u_short port)
{
	if (m_pType != ProtocolType::TCP) {
		throw std::exception("Connect is only valid for TCP Protocol.");
	}

	m_SocketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_SocketHandle == INVALID_SOCKET)
		return false;

	sockaddr_in remoteEndPoint = StringToEndPoint(peer);
	remoteEndPoint.sin_port = htons(port);

	if (::connect(m_SocketHandle, reinterpret_cast<sockaddr*>(&remoteEndPoint), sizeof(remoteEndPoint)) == SOCKET_ERROR){
		Close();
		return false;
	}

	return true;
}

bool Socket::Bind(u_short port, std::string ip)
{
	if (m_pType == ProtocolType::NONE) {
		throw std::exception("Protocol type must be set before binding.");
	}

	m_SocketHandle = socket(AF_INET, (m_pType == ProtocolType::TCP ? SOCK_STREAM : SOCK_DGRAM), 0);
	if (m_SocketHandle == INVALID_SOCKET) {
		return false;
	}

	m_HostEndPoint.sin_family = AF_INET;
	m_HostEndPoint.sin_port = htons(port);

	if (ip.empty()) {
		m_HostEndPoint.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		// IP ��ȯ 
		if (inet_pton(AF_INET, ip.c_str(), &m_HostEndPoint.sin_addr) <= 0) {
			Close();
			throw std::exception("Invalid IP address format."); // IP ��ȯ ���� �� ���� ó��
		}
	}

	// Enable Address Reuse
	int opt = 1;
	setsockopt(m_SocketHandle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

	if (bind(m_SocketHandle, reinterpret_cast<sockaddr*>(&m_HostEndPoint), sizeof(m_HostEndPoint)) == SOCKET_ERROR) {
		Close();
		return false;
	}

	return true;
}

void Socket::Close()
{
	::closesocket(m_SocketHandle);
	m_SocketHandle = INVALID_SOCKET;
}

sockaddr_in Socket::StringToEndPoint(std::string& peer)
{
	sockaddr_in addr{};
	addr.sin_family = AF_INET;

	// IP�� ��Ʈ�� ':'�� �������� �и�
	size_t colonPos = peer.find(':');
	if (colonPos == std::string::npos) {
		throw std::exception("Invalid address format, missing port.");
	}

	std::string ip = peer.substr(0, colonPos);  // IP �ּ� �κ�
	std::string portStr = peer.substr(colonPos + 1);  // ��Ʈ ��ȣ �κ�

	// IP �ּҸ� inet_pton���� ó��
	if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
		throw std::exception("Invalid IP address format.");
	}

	// ��Ʈ ��ȣ�� ����
	uint16_t port;
	std::stringstream(portStr) >> port;
	addr.sin_port = htons(port);  // ��Ʈ ��ȣ�� htons�� ����

	return addr;
}

std::string Socket::GetPeerIPandPort(const sockaddr_in& addr)
{
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ipStr, sizeof(ipStr));

	u_short port = ntohs(addr.sin_port);

	return std::string(ipStr) + ":" + std::to_string(port);
}

void Socket::PrintIPansPort()
{
	std::string ipPort = GetPeerIPandPort(m_HostEndPoint);
	std::cout << "Socket bound to: " << ipPort << std::endl;
}

bool Socket::IsValid()
{
	return m_SocketHandle != INVALID_SOCKET;
}

bool Socket::NonBlockMode()
{
	if (!IsValid()) {
		return false;
	}

	u_long mode = 1; // 1�̸� �ͺ�� ���, 0�̸� ��� ���
	if (ioctlsocket(m_SocketHandle, FIONBIO, &mode) != 0) {
		return false; 
	}

	return true; 
}
