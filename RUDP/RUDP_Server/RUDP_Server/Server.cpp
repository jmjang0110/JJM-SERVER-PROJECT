#include "pch.h"
#include "Server.h"

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

    // SendThread 함수 호출을 위한 람다 표현식 사용
    m_SendThread = std::thread([this]() {
        SendThread(m_UDPsocket);
        });

    // TODO : Send data Test And Measure Drop rate (UDP)
    sockaddr_in& host = m_UDPsocket.GetAddress();
    uint64_t data = 0;
    while (data < 20'000) {
        m_UDPsocket.RegisterSend(reinterpret_cast<std::byte*>(&data), sizeof(data), host);
        data++;
        std::cout << data << "\n";

    }

    m_SendThread.join();

}

void Server::SendThread(RUDPSocket& socket)
{
    UDPpacket sendPkt;

    while (true) {

        if (socket.GetSendPacket(sendPkt)) {
            try {
                auto peer = socket.StringToEndPoint(sendPkt.peer);
                socket.SendTo(sendPkt.data.data(), sendPkt.data.size(), peer);
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


