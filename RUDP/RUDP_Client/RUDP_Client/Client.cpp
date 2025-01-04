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
    uint64_t expectedPackets = 20000;  // ���÷� ���� ��Ŷ ���� 20,000���� ����
    uint64_t lastReceivedPacket = 0;   // ���������� ���ŵ� ��Ŷ ��ȣ ����
    uint64_t missedPackets = 0;        // ��ģ ��Ŷ ��

    while (true) {
        try {
            // ���ŵ� �����Ͱ� recvBuffer�� �����
            int receivedSize = m_UDPsocket.RecvFrom();

            if (receivedSize > 0) {
                std::cout << "Received data: ";
                // ���ŵ� ������ ũ�⸸ŭ ���
                uint64_t receivedData = 0;
                std::memcpy(&receivedData, m_UDPsocket.GetRecvBuf(), sizeof(receivedData));

                std::cout << "Received uint64_t value: " << receivedData << std::endl;

                totalReceivedPackets++;

                // ��Ŷ ��ȣ�� ����Ǵ� ������ ũ�� ��ģ ��Ŷ���� ���
                if (receivedData > lastReceivedPacket + 1) {
                    missedPackets += receivedData - lastReceivedPacket - 1;
                }

                // ���� ������ ��Ŷ ��ȣ ����
                lastReceivedPacket = receivedData;

                // Hit Rate ���
                double hitRate = static_cast<double>(totalReceivedPackets) / expectedPackets * 100;

                // Drop Rate ��� (��ģ ��Ŷ ����)
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