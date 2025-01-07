#include "RUDPSocket.h"

RUDPSocket::RUDPSocket() : Socket(ProtocolType::UDP)
{
}

RUDPSocket::~RUDPSocket()
{
}

void RUDPSocket::RegisterSend(std::byte* buf, size_t len, sockaddr_in& to) {
    std::lock_guard<std::mutex> lg(m_SendPktMutex);
    
    UDPpacket packet;
    packet.peer = GetPeerIPandPort(to);
    packet.data.resize(len);
    memcpy_s(packet.data.data(), len, buf, len);

    Q.push(packet);
   // m_SendPktsQ.push(packet);
}

bool RUDPSocket::GetSendPacket(UDPpacket& packet)
{
    std::lock_guard<std::mutex> lg(m_SendPktMutex);

    //if(m_SendPktsQ.empty()) return false;

   // m_SendPktsQ.try_pop(packet);
    if (Q.empty()) return false;
    
    packet = Q.front();
    Q.pop();
    return true;
}

void RUDPSocket::SendTo(std::byte* buf, size_t len, sockaddr_in& to) {

    int result = ::sendto(m_SocketHandle,
                        reinterpret_cast<char*>(buf),
                        static_cast<int>(len),
                        0,
                        reinterpret_cast<sockaddr*>(&to),
                        sizeof(to));

    if (result == SOCKET_ERROR) {
        int errCode = WSAGetLastError();  
        std::string errorMsg = "Failed to send data via UDP. Error Code: " + std::to_string(errCode);
        throw std::exception(errorMsg.c_str());
    }
}

int RUDPSocket::RecvFrom()
{
    sockaddr_in from;
    int senderSize = sizeof(from);
    int result = recvfrom(m_SocketHandle,
                          reinterpret_cast<char*>(m_RecvBuffer),
                          BUF_SIZE,
                          0,
                          reinterpret_cast<sockaddr*>(&from),
                          &senderSize);

    if (result == SOCKET_ERROR) {
        int errCode = WSAGetLastError();
        std::string errorMsg = "Failed to receive data via UDP. Error Code: " + std::to_string(errCode);
        throw std::exception(errorMsg.c_str());
    }

    return result;  // 수신한 데이터 크기 반환
}