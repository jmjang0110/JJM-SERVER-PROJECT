#include "pch.h"
#include "Common/RUDPSocket.h"
#include "Room.h"

bool Room::Hole_Punching(RUDPSocket& socket, int a_id, int b_id)
{


	return false;
}

void Room::Enter(sockaddr_in end_point)
{
	if (m_Idx >= 2)
		return;

	m_sessions[m_Idx] = end_point;
	m_Idx += 1;
}
