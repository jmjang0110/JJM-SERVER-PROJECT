#include "pch.h"
#include "Server.h"
#include "Common/Packet.h"

#include <map>

bool Server::Init()
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

void Server::Exit()
{
    m_UDPsocket.Close();
    ::WSACleanup();
}

void Server::Execute()
{
    //wait_ans_stop();

    go_back_N_ARQ();

}

void Server::SendThread_wait_and_stop(RUDPSocket& socket)
{
    UDPpacket sendPkt;

    while (true) {

        if (socket.GetSendPacket(sendPkt)) {
            try {
                uint64_t data = 0;
                std::memcpy(&data, sendPkt.data.data(), sizeof(data));
                auto peer = socket.StringToEndPoint(sendPkt.peer);

                while (true) {
                    // 1. send data 
                    socket.SendTo(sendPkt.data.data(), sendPkt.data.size(), peer);
                    std::cout << "Send : " << data << "\n";
                    //break;

                    // 2. recv ack packet 
                    int receivedSize = socket.RecvFrom();
                    if (receivedSize > 0) {
                        bool ack{};
                        std::memcpy(&ack, socket.GetRecvBuf(), sizeof(ack));
                        std::cout << data << " - " << "ACK : " << ack << "\n";
                        if (ack == true)
                            break;
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error sending packet: " << e.what() << std::endl;
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


    }

    std::cout << "Send thread has finished." << std::endl;
}



void Server::wait_ans_stop()
{
    // SendThread 함수 호출을 위한 람다 표현식 사용
    m_SendThread = std::thread([this]() {
        SendThread_wait_and_stop(m_UDPsocket);
        });

    // TODO : Send data Test And Measure Drop rate (UDP)
    sockaddr_in peer = m_UDPsocket.Peer(CLIENT_IP, CLIENT_PORT);
    uint64_t data = 0;
    while (data < 20'000) {
        m_UDPsocket.RegisterSend(reinterpret_cast<std::byte*>(&data), sizeof(data), peer);
        data++;
        std::cout << data << "\n";

    }

    m_SendThread.join();
}


namespace STATE_TYPE {
    constexpr int handShake  = 1;
    constexpr int recvSynpkt = 2;
    constexpr int sendData   = 3;
    constexpr int requestFIN = 4;
};

void Server::go_back_N_ARQ()
{
    sockaddr_in peer = m_UDPsocket.Peer(CLIENT_IP, CLIENT_PORT);

    if (!m_UDPsocket.NonBlockMode()) {
        std::cout << "UDP socket Nonblock Mode setting Failed!\n";
        return;
    }

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
        case STATE_TYPE::handShake: {

            dataPkt = Create_SYN_pkt(0);
            m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&dataPkt), sizeof(dataPkt), peer);
            std::cout << "Establishing Connection to Receiver --- (sending SYN packet)\n";

            state = STATE_TYPE::recvSynpkt;

        }
        break;

        case STATE_TYPE::recvSynpkt: {

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
                    state = STATE_TYPE::sendData;
                }
            }
        }
        break;
        case STATE_TYPE::sendData: {

            // 윈도우 내 전송되지 않은 패킷 전송 
            if (curSeq < windowbase + WINDOW_SIZE && curSeq <= lastSeq) {
                dataPkt = Create_DATA_pkt(curSeq, static_cast<long>(curSeq));
                std::cout << "Send Data packet seq : " << curSeq << "\n";
                m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&dataPkt), sizeof(dataPkt), peer);
                curSeq++;
            }

            recvResult = m_UDPsocket.RecvFrom();
            if (recvResult < 0) {
                // 시간이 지났는지 체크
                auto now = std::chrono::steady_clock::now();
                if (now - lastTime > timeout_ms) {
                    std::cout << "Window Time out...\n";
                    lastTime = now; // 타임아웃 이후 시간을 갱신
                    curSeq = curAck; // 시퀀스를 재설정
                }
            }
            else {
                std::memcpy(&ackPkt, m_UDPsocket.GetRecvBuf(), sizeof(ackPkt));
                if (curAck <= ackPkt.seq) {
                    std::cout << "ACK Received : SEQ " << ackPkt.seq << "\n";
                    curAck = ackPkt.seq + 1;
                    windowbase = curAck;
                    lastTime = std::chrono::steady_clock::now(); // ACK 수신 시 타이머 리셋
                }
            }

            // 전부 받음 
            if (curAck > lastSeq) {
                state = STATE_TYPE::requestFIN;
                dataPkt = Create_FIN_pkt(curAck);
                std::cout << "Terminate Connection... (Send FIN Packet)\n";
                m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&dataPkt), sizeof(dataPkt), peer);
            }

        }
        break;
        case STATE_TYPE::requestFIN: {

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

    std::cout << "Go-Back-N-ARQ exit successfully\n";
}
