#include "pch.h"
#include "Client.h"

bool Client::Init()
{
    WSAData wsadata{};
    if (::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        throw std::exception("Failed WSAStartup.");
    }

    m_UDPsocket.Bind(SERVER_PORT, SERVER_IP);
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
    uint64_t totalReceivedPackets = 0;
    uint64_t expectedPackets = 20000;  // 예시로 예상 패킷 수를 20,000으로 설정
    uint64_t lastReceivedPacket = 0;   // 마지막으로 수신된 패킷 번호 추적
    uint64_t missedPackets = 0;        // 놓친 패킷 수

    while (true) {
        try {
            // 수신된 데이터가 recvBuffer에 저장됨
            int receivedSize = m_UDPsocket.RecvFrom();

            if (receivedSize > 0) {
                std::cout << "Received data: ";
                // 수신된 데이터 크기만큼 출력
                uint64_t receivedData = 0;
                std::memcpy(&receivedData, m_UDPsocket.GetRecvBuf(), sizeof(receivedData));

                std::cout << "Received uint64_t value: " << receivedData << std::endl;

                totalReceivedPackets++;

                // 패킷 번호가 예상되는 값보다 크면 놓친 패킷으로 계산
                if (receivedData > lastReceivedPacket + 1) {
                    missedPackets += receivedData - lastReceivedPacket - 1;
                }

                // 받은 마지막 패킷 번호 갱신
                lastReceivedPacket = receivedData;

                // Hit Rate 계산
                double hitRate = static_cast<double>(totalReceivedPackets) / expectedPackets * 100;

                // Drop Rate 계산 (놓친 패킷 비율)
                double dropRate = static_cast<double>(missedPackets) / expectedPackets * 100;

                std::cout << "Hit Rate: " << hitRate << "%" << std::endl;
                std::cout << "Drop Rate: " << dropRate << "%" << std::endl;
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

    std::cout << "Receive thread has finished." << std::endl;
}