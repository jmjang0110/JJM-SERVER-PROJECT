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
    // SERVER_PORT와 IP 주소 설정

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
    constexpr int WINDOW_SIZE = 50; // 윈도우 크기
    uint64_t max_num = 20'000; // 수신할 최대 패킷 수
    uint64_t expected_packet_idx = 0; // 예상되는 패킷 번호
    std::atomic<int> window_base_idx = 0; // 윈도우의 시작 인덱스
    std::vector<uint64_t> window(WINDOW_SIZE, 0); // 윈도우에 들어갈 데이터 저장용 벡터

    sockaddr_in server = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT); // 서버 주소 설정
    auto lastsendtime = std::chrono::steady_clock::now(); // 마지막 전송 시간 초기화

    auto start_time = std::chrono::steady_clock::now(); // 실행 시작 시간 기록

    while (expected_packet_idx < max_num) {
        // 1. 서버로부터 패킷 수신
        int receivedSize = m_UDPsocket.RecvFrom();
        if (receivedSize > 0) {
            uint64_t packet_data = 0;
            std::memcpy(&packet_data, m_UDPsocket.GetRecvBuf(), sizeof(packet_data));

            // 2. 수신한 패킷이 예상되는 번호와 일치하면 ACK 전송
            if (packet_data == expected_packet_idx) {
                std::cout << "수신한 패킷 번호: " << packet_data << "\n";

                // ACK 전송
                SendAckPacket_go_Back_N_ARQ(packet_data);
                lastsendtime = std::chrono::steady_clock::now(); // 마지막 전송 시간 갱신

                // 윈도우 내의 패킷 번호 업데이트
                window[packet_data % WINDOW_SIZE] = packet_data;
                expected_packet_idx++;

                // 윈도우 베이스 이동
                while (window[window_base_idx % WINDOW_SIZE] == window_base_idx) {
                    window[window_base_idx % WINDOW_SIZE] = 0; // ACK 처리된 슬롯 초기화
                    window_base_idx++;
                }
            }
            else {
                std::cout << "잘못된 패킷 번호 수신: " << packet_data << "\n";
            }
        }

        // 3. 타임아웃 체크 및 재전송 요청
        auto now = std::chrono::steady_clock::now();
        if (now - lastsendtime >= std::chrono::milliseconds(600)) {
            for (int i = 0; i < WINDOW_SIZE; ++i) {
                uint64_t packet_data = window[(window_base_idx + i) % WINDOW_SIZE];
                if (packet_data == 0) continue; // ACK 처리된 패킷은 건너뛰기
                m_UDPsocket.RegisterSend(reinterpret_cast<std::byte*>(&packet_data), sizeof(packet_data), server);
                std::cout << "재전송 요청: " << packet_data << "\n";
            }
            lastsendtime = now; // 타임아웃 시간 초기화
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 잠시 대기 후 다시 확인
    }

    // 4. 소요 시간 출력
    auto end_time = std::chrono::steady_clock::now(); // 실행 종료 시간 기록
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "클라이언트 go_back_N_ARQ 완료. 소요 시간: " << duration << "ms\n";
}
