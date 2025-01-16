#include "pch.h"
#include "Client.h"
#include "Common/Packet.h"




bool Client::Init()
{
	WSAData wsadata{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		throw std::exception("Failed WSAStartup.");
	}

	m_UDPsocket.Bind(CLIENT_PORT, CLIENT2_IP);
	std::cout << "------------- UDP SOCKET IP, Port -------------\n";
	m_UDPsocket.PrintIPansPort();
	std::cout << "-----------------------------------------------\n";

	return true;
}

void Client::Exit()
{
	m_UDPsocket.Close();
	::WSACleanup();
}

void Client::Execute()
{
	//wait_and_stop();


   // go_back_N_ARQ();


	UDP_HolePunching();

}

void Client::RecvNum(uint64_t data)
{
	std::cout << "Received uint64_t value: " << data << std::endl;
	m_recvNums.push_back(data);
}

void Client::SendAckPacket()
{
	// SERVER_PORT와 IP 주소 설정

	sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);
	bool ack = true;
	m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ack), sizeof(ack), peer);
}

void Client::wait_and_stop()
{
	std::set<uint64_t> missedNums; // 중복을 방지하기 위해 set 사용
	uint64_t lastReceived = -1; // 마지막으로 수신한 패킷 번호, 초기값은 -1로 설정
	const uint64_t endPacket = 19999; // 마지막 패킷 번호
	auto startTime = std::chrono::high_resolution_clock::now(); // 시작 시간 기록

	while (true) {
		try {
			// recv 
			int receivedSize = m_UDPsocket.RecvFrom();
			if (receivedSize > 0) {
				// 수신된 데이터 크기만큼 출력
				uint64_t receivedData = 0;
				std::memcpy(&receivedData, m_UDPsocket.GetRecvBuf(), sizeof(receivedData));
				RecvNum(receivedData);

				// 누락된 패킷 확인
				if (lastReceived != -1 && receivedData != lastReceived + 1) {
					for (uint64_t i = lastReceived + 1; i < receivedData; ++i) {
						missedNums.insert(i); // set에 추가하여 중복 방지
					}
				}

				lastReceived = receivedData; // 마지막 수신된 패킷 업데이트

				std::cout << "missed packet size : " << missedNums.size() << "\n";

				// 19999번 패킷 수신 시 종료
				if (receivedData == endPacket) {
					break;
				}

				// send 
				SendAckPacket();
			}
			else {
				std::cerr << "No data received or error occurred." << std::endl;
			}

		}
		catch (const std::exception& e) {
			std::cerr << "Error receiving packet: " << e.what() << std::endl;
			break;
		}
	}

	auto endTime = std::chrono::high_resolution_clock::now(); // 종료 시간 기록
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count(); // 밀리초 단위로 시간 계산

	std::cout << "Missed Packets: ";
	for (const auto& missed : missedNums) {
		std::cout << missed << " ";
	}
	std::cout << std::endl;

	std::cout << "Receive thread has finished." << std::endl;

	std::cout << "Time taken to receive 0 ~ 19999 packets: " << duration << " ms" << std::endl;
}


#include <chrono> // Include the chrono header for time tracking

void Client::go_back_N_ARQ()
{
	if (!m_UDPsocket.NonBlockMode()) {
		std::cout << "UDP Socket Nonbock Mode Failed \n";
		return;
	}

	dataPacket dataPkt;
	ackPacket  ackPkt;

	sockaddr_in peerInfo;
	sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);

	int status{};
	int seq = 0;
	int wait_seq = 0;
	int synFlag = 0;
	int ackFlag = 0;
	int finFlag = 0;
	int wait_time = 0;

	bool loopflag = 1;

	bool Established = false;

	// Start measuring time
	auto start_time = std::chrono::steady_clock::now();

	while (loopflag) {

		/************* 종료 ************/
		if (finFlag == 1) {
			wait_time++;
			if (wait_time > 1000) {
				std::cout << "Closed ! time Up \n";
				loopflag = 0;
			}
		}

		status = m_UDPsocket.RecvFrom();
		if (status < 0) {

		}
		else {
			std::memcpy(&dataPkt, m_UDPsocket.GetRecvBuf(), sizeof(dataPkt));

			/************* SYN ************/
			if (dataPkt.type == PKT_TYPE::SYN && dataPkt.seq == 0) {

				std::string peer_ip_port = m_UDPsocket.GetPeerIPandPort(peerInfo);
				std::cout << "Connection Request Received From " << peer_ip_port << "\n";
				ackPkt = Create_SYN_ACK_pkt(seq);

				std::cout << "Send ACK with SEQ : " << seq << " Expecting SEQ : " << wait_seq << "\n";
				m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
				synFlag = 1;
			}

			else if (dataPkt.type == PKT_TYPE::ACK && dataPkt.seq == 0) {

				std::string peer_ip_port = m_UDPsocket.GetPeerIPandPort(peerInfo);
				std::cout << "Connection Request Received From " << peer_ip_port << "\n";
				seq = dataPkt.seq;
				wait_seq = seq + 1;
				ackFlag = 1;
				Established = true;
			}

			/************* DATA ************/
			else if (dataPkt.type == PKT_TYPE::DATA && dataPkt.seq > 0 && Established) {

				if (dataPkt.seq == wait_seq) {
					wait_seq++;
					seq = dataPkt.seq;

					ackPkt = Create_DATA_ACK_pkt(seq);
					std::cout << "Received data : " << dataPkt.data << "\n";
					std::cout << "send ACK with seq  " << seq << " expecting seq : " << wait_seq << "\n";
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);

				}
				else {
					std::cout << "out of order seq : " << dataPkt.seq << "\n";
					std::cout << "send ACK with seq : " << wait_seq - 1 << "\n";
					ackPkt = Create_DATA_ACK_pkt(wait_seq - 1);
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
				}

			}

			/************* FIN ************/
			else if (dataPkt.type == PKT_TYPE::FIN && synFlag == 1) {
				ackPkt = Create_FIN_ACK_pkt(dataPkt.seq);

				if (finFlag == 0) {
					std::cout << "Sender is terminating with FIN...\n ";
					std::cout << "Send ACK with seq : " << dataPkt.seq << "\n";
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
				}
				else if (finFlag == 1) {
					std::cout << "Sender is terminating with FIN... \n";
					std::cout << "sending ACK with SEQ : " << dataPkt.seq << "\n";
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
				}

				finFlag = 1;
			}
		}
	}

	// Stop measuring time
	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end_time - start_time;

	// Output elapsed time
	std::cout << "Total time taken: " << elapsed_seconds.count() << " seconds\n";
}

void Client::UDP_HolePunching()
{
	sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);

	if (!m_UDPsocket.NonBlockMode()) {
		std::cout << "UDP socket Nonblock Mode setting Failed!\n";
		return;
	}

	PacketHeader header;
	UDPholePunchingPacket hpcPkt;
	sockaddr_in remote_endpoint;


	dataPacket dataPkt; // 데이터 패킷 구조체
	ackPacket  ackPkt;  // ACK 패킷 구조체

	int recvResult{};   // 수신 결과 저장

	int state = STATE_TYPE::handShake; // 현재 상태 (초기 상태는 핸드셰이크)
	bool loopflag = true; // 메인 루프 플래그

	int curSeq = 0; // 현재 시퀀스 번호
	int curAck = 0; // 현재 ACK 번호
	int lastSeq = 20'000 - 1;
	int windowbase = 0;

	// 시간 초과를 위한 chrono
	auto lastTime = std::chrono::steady_clock::now();
	std::chrono::milliseconds timeout_ms(600);  // 타임아웃 설정 (600ms)

	while (loopflag) {

		switch (state)
		{
		case STATE_TYPE::handShake:
		{
			dataPkt = Create_SYN_pkt(0);
			m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&dataPkt), sizeof(dataPkt), peer);
			state = STATE_TYPE::recvSynpkt;
		}
		break;

		case STATE_TYPE::recvSynpkt:
		{

			recvResult = m_UDPsocket.RecvFrom();
			if (recvResult < 0) {
				// 시간이 지났는지 체크
				auto now = std::chrono::steady_clock::now();
				if (now - lastTime > timeout_ms) {
					std::cout << "SYN - Time Out...\n";
					lastTime = now; // 타임아웃 이후 시간을 갱신
					state = STATE_TYPE::handShake; // 핸드셰이크 재시도 
				}
			}
			else {
				std::memcpy(&ackPkt, m_UDPsocket.GetRecvBuf(), sizeof(ackPkt));
				if (ackPkt.seq == 0 && ackPkt.type == PKT_TYPE::SYN_ACK) {
					lastTime = std::chrono::steady_clock::now(); // 성공적인 ACK 수신 시 타이머 리셋

					ackPkt = Create_ACK_pkt(0);
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
					state = STATE_TYPE::holepunching;
				}
			}
		}
		break;
		case STATE_TYPE::holepunching:
		{
			recvResult = m_UDPsocket.RecvFrom();
			if (recvResult < 0) {
				// 시간이 지났는지 체크
				auto now = std::chrono::steady_clock::now();
				if (now - lastTime > timeout_ms) {
					std::cout << "holepunching - Time Out...\n";
					lastTime = now; // 타임아웃 이후 시간을 갱신
				}
			}
			else {
				std::memcpy(&header, m_UDPsocket.GetRecvBuf(), sizeof(header));
				if (header.seq == 0 && header.type == PKT_TYPE::HOLE_PUNCING) {
					lastTime = std::chrono::steady_clock::now(); // 성공적인 ACK 수신 시 타이머 리셋
					std::memcpy(&hpcPkt, m_UDPsocket.GetRecvBuf(), sizeof(hpcPkt));

					remote_endpoint = hpcPkt.peer_end_point;

					auto pkt = Create_UDPhpc_Pkt(remote_endpoint, '1');
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&pkt), sizeof(pkt), peer);

					auto synpkt = Create_P2Pack_Pkt();
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&synpkt), sizeof(synpkt), remote_endpoint);
					
					state = STATE_TYPE::waitForSynAck; // 상대 클라이언트와 핸드쉐이크

				}
			}

		}
		break;
		case STATE_TYPE::waitForSynAck:
		{
			recvResult = m_UDPsocket.RecvFrom();
			if (recvResult < 0) {
				// 시간이 지났는지 체크
				auto now = std::chrono::steady_clock::now();
				if (now - lastTime > timeout_ms) {
					std::cout << m_UDPsocket.GetPeerIPandPort(remote_endpoint) << " - SYN - Time Out...\n";
					lastTime = now; // 타임아웃 이후 시간을 갱신
					auto synpkt = Create_SYN_pkt(0);
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&synpkt), sizeof(synpkt), remote_endpoint);
				}
			}
			else {
				// P2P_ACK 수신 대기
				std::memcpy(&header, m_UDPsocket.GetRecvBuf(), sizeof(header));
				if (header.type == PKT_TYPE::P2P_ACK) {
					std::cout << "P2P_ACK Received from " << m_UDPsocket.GetPeerIPandPort(remote_endpoint) << "\n";
					state = STATE_TYPE::sendData;
				}
			}
		}
		break;
		case STATE_TYPE::sendData:
		{
			recvResult = m_UDPsocket.RecvFrom();
			if (recvResult < 0) {
			}
			else {
				// P2P_ACK 수신 대기
				std::memcpy(&header, m_UDPsocket.GetRecvBuf(), sizeof(header));
				if (header.type == PKT_TYPE::P2P_DATA) {
					P2PDataPacket pkt;

					std::memcpy(&pkt, m_UDPsocket.GetRecvBuf(), sizeof(pkt));
					std::cout << "P2P_DATA Received from " << m_UDPsocket.GetPeerIPandPort(remote_endpoint) << " : " << pkt.data << "\n";
				}
			}

			auto now = std::chrono::steady_clock::now();
			static auto lastSendTime = std::chrono::steady_clock::now();  // Keeps track of last send time
			std::chrono::milliseconds interval(1000);  // 3 seconds

			if (now - lastSendTime >= interval) {
				char str[20] = "Hello I'm P2";
				P2PDataPacket data_pkt = CreateP2Pdata_Pkt(str);
				m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&data_pkt), sizeof(data_pkt), remote_endpoint);

				std::cout << "Sent P2P Data: " << str << "\n";
				lastSendTime = now; 
			}
		}
		break;
		case STATE_TYPE::requestFIN:
		{

			recvResult = m_UDPsocket.RecvFrom();
			if (recvResult < 0) {
				// 시간이 지났는지 체크
				auto now = std::chrono::steady_clock::now();
				if (now - lastTime > timeout_ms) {
					std::cout << "FIN packet Time out! Resend...\n";
					lastTime = now; // 타임아웃 이후 시간을 갱신
					dataPkt = Create_FIN_pkt(curSeq);
					std::cout << "Terminate Connection... (Send FIN)\n";
					m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&dataPkt), sizeof(dataPkt), peer);
				}

			}
			else {
				std::memcpy(&ackPkt, m_UDPsocket.GetRecvBuf(), sizeof(ackPkt));
				if (ackPkt.seq == (lastSeq + 1) && ackPkt.type == PKT_TYPE::ACK_FIN) {
					std::cout << "ACK Received SEQ : " << ackPkt.seq << "\n";
					loopflag = false;
				}
			}
		}
		break;

		default:
			break;
		}

	}


}

