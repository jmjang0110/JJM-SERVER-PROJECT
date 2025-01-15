#include "pch.h"
#include "Client.h"

bool Client::Init()
{
    WSAData wsadata{};
    if (::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        throw std::exception("Failed WSAStartup.");
    }

    m_UDPsocket.Bind(CLIENT_PORT, CLIENT_IP);
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


    go_back_N_ARQ();


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

void Client::go_back_N_ARQ()
{
    if (!m_UDPsocket.NonBlockMode()) {
        std::cout << "UDP Socket Nonbock Mode Failed \n";
        return;
    }


}
