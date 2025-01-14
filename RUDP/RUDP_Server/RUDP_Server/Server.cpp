#include "pch.h"
#include "Server.h"
#include <map>

std::atomic<int> Server::m_window_base_idx = 0;
std::atomic<uint64_t> Server::m_data = 0;
std::atomic<std::chrono::steady_clock::time_point> Server::m_lastSendTime;

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
    wait_ans_stop();

    //go_back_N_ARQ();

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

void Server::SendThread_go_back_N_ARQ(RUDPSocket& socket)
{
    sockaddr_in peer = socket.Peer(CLIENT_IP, CLIENT_PORT);
    constexpr int WINDOW_SIZE = 50;


    uint64_t max_num = 20'000;

    while (m_window_base_idx.load() < max_num) {

        // 1. 데이터들을 전부 전송한다.

        // window base : 0 + WINDOW_SIZE 5
        // 0 1 2 3 4
        while (m_data.load() < m_window_base_idx.load() + WINDOW_SIZE) {
            socket.RegisterSend(reinterpret_cast<std::byte*>(&m_data), sizeof(m_data.load()), peer);
            m_data.fetch_add(1);
        }


    }
}

void Server::RecvThread_go_back_N_ARQ(RUDPSocket& socket)
{
    while (true) {
        int receivedSize = socket.RecvFrom();
        if (receivedSize > 0) {
            // check ACK number 
            uint64_t ack{};
            std::memcpy(&ack, socket.GetRecvBuf(), sizeof(ack));

            if (m_window_base_idx.load() == ack) {
                m_window_base_idx.fetch_add(1);
                std::cout << "Recv ACK : " << ack << "\n";
            }
            // 패킷이 유실된 경우 데이터를 유실된 지점부터 재전송한다. 
            else {
                m_data = m_window_base_idx.load();
            }

            // 시간초과(600ms)가 난 경우 
            auto now = std::chrono::steady_clock::now();
            if (now - m_lastSendTime.load() >= std::chrono::milliseconds(600)) {
                // 타임아웃 발생, 재전송
                m_data.store(m_window_base_idx.load());
            }
        }
    }
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

void Server::go_back_N_ARQ()
{

    // SendThread 함수 호출을 위한 람다 표현식 사용
    m_SendThread = std::thread([this]() {
        SendThread_go_back_N_ARQ(m_UDPsocket);
        });

    m_RecvACKThread = std::thread([this]() {
        RecvThread_go_back_N_ARQ(m_UDPsocket);
        });


    // TODO : Send data Test And Measure Drop rate (UDP)
    UDPpacket sendPkt;

    while (true) {

        if (m_UDPsocket.GetSendPacket(sendPkt)) {
            try {
                uint64_t data = 0;
                std::memcpy(&data, sendPkt.data.data(), sizeof(data));
                auto peer = m_UDPsocket.StringToEndPoint(sendPkt.peer);

                // 1. send data 
                m_UDPsocket.SendTo(sendPkt.data.data(), sendPkt.data.size(), peer);
                std::cout << "Send : " << data << "\n";
                m_lastSendTime = std::chrono::steady_clock::now();
            }
            catch (const std::exception& e) {
                std::cerr << "Error sending packet: " << e.what() << std::endl;
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


    }

    std::cout << "go_back_N_ARQ thread has finished." << std::endl;
    std::cout << "Send thread has finished." << std::endl;
    std::cout << "Recv thread has finished." << std::endl;

    m_SendThread.join();
    m_RecvACKThread.join();


}


