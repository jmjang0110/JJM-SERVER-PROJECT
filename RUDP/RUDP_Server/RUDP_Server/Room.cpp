#include "pch.h"
#include "Common/RUDPSocket.h"
#include "Room.h"
#include "Common/Packet.h"

bool Room::Hole_Punching(RUDPSocket& socket)
{
	static auto lastExecutionTime = std::chrono::steady_clock::now(); // 이전 실행 시간 기록

	// 현재 시간과 마지막 실행 시간 간격 계산
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastExecutionTime);

	if (elapsedTime.count() < 2000) {
		return false;
	}

	if (m_Idx < 2)
		return false;

	if (m_sessions[1].ready_to_holepunching == false) {
		auto to_0 = Create_UDPhpc_Pkt(m_sessions[1].peer, '0');
		socket.SendTo(reinterpret_cast<std::byte*>(&to_0), sizeof(to_0), m_sessions[0].peer);
	}
	
	if (m_sessions[0].ready_to_holepunching == false) {
		auto to_1 = Create_UDPhpc_Pkt(m_sessions[0].peer, '0');
		socket.SendTo(reinterpret_cast<std::byte*>(&to_1), sizeof(to_1), m_sessions[1].peer);
	}

	
	return true;
}

void Room::Hole_punching_Ready_Ack(sockaddr_in peer)
{
	RUDPSocket sock;
	for (int i = 0; i < m_Idx; ++i) {
		if (sock.GetPeerIPandPort(m_sessions[i].peer) == sock.GetPeerIPandPort(peer)) {
			m_sessions[i].ready_to_holepunching = true;
			break;
		}
	}
}

void Room::Enter(sockaddr_in end_point)
{
	if (m_Idx >= 2)
		return;

	m_sessions[m_Idx].peer = end_point;
	m_Idx += 1;
}
