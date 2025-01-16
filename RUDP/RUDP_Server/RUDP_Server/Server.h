#pragma once

class Server
{
private:
	RUDPSocket	m_UDPsocket;
	std::thread m_SendThread;
	bool		m_SendThreadLoop = true;

	RUDPSocket	m_peerSocket;

	class Room*  m_sessionRoom;

public:
	bool Init();
	void Exit();
	void Execute();

private:
	static void SendThread_wait_and_stop(RUDPSocket& socket);


private:
	void wait_ans_stop();
	void go_back_N_ARQ();
	void UDP_HolePunching();

};


