#pragma once

class Room
{
private:
	std::array<sockaddr_in, 2> m_sessions{};
	int m_Idx = 0;

public:
	bool Hole_Punching(class RUDPSocket& socket, int a_id, int b_id);

	void Enter(sockaddr_in end_point);

};

