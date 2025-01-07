#pragma once

class Server
{
private:
	RUDPSocket m_UDPsocket;
	std::thread m_SendThread;
	bool m_SendThreadLoop = true;

public:
	bool Init();
	void Exit();
	void Execute();

private:
	static void SendThread(RUDPSocket& socket);
	void RecvAckPacket(RUDPSocket& socket, UDPpacket& sendpkt);

};

