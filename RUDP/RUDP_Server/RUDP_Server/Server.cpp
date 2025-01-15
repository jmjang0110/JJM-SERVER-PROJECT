#include "pch.h"
#include "Server.h"
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

void Server::go_back_N_ARQ()
{


}

