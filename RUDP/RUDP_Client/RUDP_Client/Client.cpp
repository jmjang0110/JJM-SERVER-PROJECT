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
    wait_and_stop();


    //go_back_N_ARQ();


}

void Client::RecvNum(uint64_t data)
{
    std::cout << "Received uint64_t value: " << data << std::endl;
    m_recvNums.push_back(data);
}

void Client::SendAckPacket()
{
    // SERVER_PORT�� IP �ּ� ����

    sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);
    bool ack = true;
    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ack), sizeof(ack), peer);
}

void Client::SendAckPacket_go_Back_N_ARQ(uint64_t data)
{
    sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);
    uint64_t ack = data;
    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ack), sizeof(ack), peer);
}

void Client::wait_and_stop()
{
    std::set<uint64_t> missedNums; // �ߺ��� �����ϱ� ���� set ���
    uint64_t lastReceived = -1; // ���������� ������ ��Ŷ ��ȣ, �ʱⰪ�� -1�� ����
    const uint64_t endPacket = 19999; // ������ ��Ŷ ��ȣ
    auto startTime = std::chrono::high_resolution_clock::now(); // ���� �ð� ���

    while (true) {
        try {
            // recv 
            int receivedSize = m_UDPsocket.RecvFrom();
            if (receivedSize > 0) {
                // ���ŵ� ������ ũ�⸸ŭ ���
                uint64_t receivedData = 0;
                std::memcpy(&receivedData, m_UDPsocket.GetRecvBuf(), sizeof(receivedData));
                RecvNum(receivedData);

                // ������ ��Ŷ Ȯ��
                if (lastReceived != -1 && receivedData != lastReceived + 1) {
                    for (uint64_t i = lastReceived + 1; i < receivedData; ++i) {
                        missedNums.insert(i); // set�� �߰��Ͽ� �ߺ� ����
                    }
                }

                lastReceived = receivedData; // ������ ���ŵ� ��Ŷ ������Ʈ

                std::cout << "missed packet size : " << missedNums.size() << "\n";

                // 19999�� ��Ŷ ���� �� ����
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

    auto endTime = std::chrono::high_resolution_clock::now(); // ���� �ð� ���
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count(); // �и��� ������ �ð� ���

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
    constexpr int WINDOW_SIZE = 50; // ������ ũ��
    uint64_t max_num = 20'000; // ������ �ִ� ��Ŷ ��
    uint64_t expected_packet_idx = 0; // ����Ǵ� ��Ŷ ��ȣ
    std::atomic<int> window_base_idx = 0; // �������� ���� �ε���
    std::vector<uint64_t> window(WINDOW_SIZE, 0); // �����쿡 �� ������ ����� ����

    sockaddr_in server = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT); // ���� �ּ� ����
    auto lastsendtime = std::chrono::steady_clock::now(); // ������ ���� �ð� �ʱ�ȭ

    auto start_time = std::chrono::steady_clock::now(); // ���� ���� �ð� ���

    while (expected_packet_idx < max_num) {
        // 1. �����κ��� ��Ŷ ����
        int receivedSize = m_UDPsocket.RecvFrom();
        if (receivedSize > 0) {
            uint64_t packet_data = 0;
            std::memcpy(&packet_data, m_UDPsocket.GetRecvBuf(), sizeof(packet_data));

            // 2. ������ ��Ŷ�� ����Ǵ� ��ȣ�� ��ġ�ϸ� ACK ����
            if (packet_data == expected_packet_idx) {
                std::cout << "������ ��Ŷ ��ȣ: " << packet_data << "\n";

                // ACK ����
                SendAckPacket_go_Back_N_ARQ(packet_data);
                lastsendtime = std::chrono::steady_clock::now(); // ������ ���� �ð� ����

                // ������ ���� ��Ŷ ��ȣ ������Ʈ
                window[packet_data % WINDOW_SIZE] = packet_data;
                expected_packet_idx++;

                // ������ ���̽� �̵�
                while (window[window_base_idx % WINDOW_SIZE] == window_base_idx) {
                    window[window_base_idx % WINDOW_SIZE] = 0; // ACK ó���� ���� �ʱ�ȭ
                    window_base_idx++;
                }
            }
            else {
                std::cout << "�߸��� ��Ŷ ��ȣ ����: " << packet_data << "\n";
            }
        }

        // 3. Ÿ�Ӿƿ� üũ �� ������ ��û
        auto now = std::chrono::steady_clock::now();
        if (now - lastsendtime >= std::chrono::milliseconds(600)) {
            for (int i = 0; i < WINDOW_SIZE; ++i) {
                uint64_t packet_data = window[(window_base_idx + i) % WINDOW_SIZE];
                if (packet_data == 0) continue; // ACK ó���� ��Ŷ�� �ǳʶٱ�
                m_UDPsocket.RegisterSend(reinterpret_cast<std::byte*>(&packet_data), sizeof(packet_data), server);
                std::cout << "������ ��û: " << packet_data << "\n";
            }
            lastsendtime = now; // Ÿ�Ӿƿ� �ð� �ʱ�ȭ
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // ��� ��� �� �ٽ� Ȯ��
    }

    // 4. �ҿ� �ð� ���
    auto end_time = std::chrono::steady_clock::now(); // ���� ���� �ð� ���
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Ŭ���̾�Ʈ go_back_N_ARQ �Ϸ�. �ҿ� �ð�: " << duration << "ms\n";
}
