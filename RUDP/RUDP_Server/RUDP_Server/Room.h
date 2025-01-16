#pragma once

struct Session {
	sockaddr_in peer;
	bool ready_to_holepunching = false;
};
class Room
{
private:
	std::array<Session, 2> m_sessions{};
	bool m_holePunching_Success = false;
	int m_Idx = 0;

public:
	bool Hole_Punching(class RUDPSocket& socket);
	void Hole_punching_Ready_Ack(sockaddr_in peer);

	void Enter(sockaddr_in end_point);

};

