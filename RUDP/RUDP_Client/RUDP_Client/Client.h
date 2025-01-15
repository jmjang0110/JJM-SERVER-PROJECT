#pragma once
class Client
{
private:
	RUDPSocket m_UDPsocket;

	std::vector<uint64_t> m_recvNums;
	int m_lastRecvedData = 0;


public:
	bool Init();
	void Exit();
	void Execute();

private:
	void RecvNum(uint64_t data);
	void SendAckPacket();

private:
	void wait_and_stop();
	void go_back_N_ARQ();

};

