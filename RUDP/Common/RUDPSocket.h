#pragma once
#include "Socket.h"
#include <vector>
#include <queue>
#include <concurrent_queue.h>
#include <mutex>

constexpr uint16_t BUF_SIZE = 256;
struct UDPpacket {
	std::string peer;
	std::vector<std::byte> data;
};

class RUDPSocket : public Socket
{
private:
	std::byte m_RecvBuffer[BUF_SIZE]{};
	//Concurrency::concurrent_queue<UDPpacket> m_SendPktsQ;
	
	std::mutex m_SendPktMutex;
	std::queue<UDPpacket> Q;

	struct sockaddr_in m_recent_peer;

public:
	RUDPSocket();
	virtual ~RUDPSocket();

public:
	void RegisterSend(std::byte* buf, size_t len, sockaddr_in& to);
	bool GetSendPacket(UDPpacket& packet);

	void SendTo(std::byte* buf, size_t len, sockaddr_in& to);
	int RecvFrom();

	std::byte* GetRecvBuf() { return m_RecvBuffer; }
	sockaddr_in Peer(const std::string& peer_ip, const u_short& peer_port);
	sockaddr_in GetRecentPeer() { return m_recent_peer; }
};

