#pragma once

class Server
{
private:
	RUDPSocket m_UDPsocket;
	std::thread m_SendThread;
	std::thread m_RecvACKThread;

	bool m_SendThreadLoop = true;

	RUDPSocket m_peerSocket;

	/* go - back - N - ARQ */
	static std::atomic<int> m_window_base_idx;
	static std::atomic<uint64_t> m_data;
	static std::atomic<std::chrono::steady_clock::time_point> m_lastSendTime;

public:
	bool Init();
	void Exit();
	void Execute();

private:
	static void SendThread_wait_and_stop(RUDPSocket& socket);
	static void SendThread_go_back_N_ARQ(RUDPSocket& socket);
	static void RecvThread_go_back_N_ARQ(RUDPSocket& socket);


private:
	void wait_ans_stop();
	void go_back_N_ARQ();

};


