#include "pch.h"
#include "Client.h"
#include "Common/Packet.h"




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


#include <chrono> // Include the chrono header for time tracking

void Client::go_back_N_ARQ()
{
    if (!m_UDPsocket.NonBlockMode()) {
        std::cout << "UDP Socket Nonbock Mode Failed \n";
        return;
    }

    dataPacket dataPkt;
    ackPacket  ackPkt;

    sockaddr_in peerInfo;
    sockaddr_in peer = m_UDPsocket.Peer(SERVER_IP, SERVER_PORT);

    int status{};
    int seq = 0;
    int wait_seq = 0;
    int synFlag = 0;
    int finFlag = 0;
    int wait_time = 0;

    bool loopflag = 1;

    // Start measuring time
    auto start_time = std::chrono::steady_clock::now();

    while (loopflag) {

        /************* ���� ************/
        if (finFlag == 1) {
            wait_time++;
            if (wait_time > 1000) {
                std::cout << "Closed ! time Up \n";
                loopflag = 0;
            }
        }

        status = m_UDPsocket.RecvFrom();
        if (status < 0) {

        }
        else {
            std::memcpy(&dataPkt, m_UDPsocket.GetRecvBuf(), sizeof(dataPkt));

            /************* SYN ************/
            if (dataPkt.type == PKT_TYPE::SYN && dataPkt.seq == 0) {

                std::string peer_ip_port = m_UDPsocket.GetPeerIPandPort(peerInfo);
                std::cout << "Connection Request Received From " << peer_ip_port << "\n";
                ackPkt = Create_SYN_ACK_pkt(seq);
                seq = dataPkt.seq;
                wait_seq = seq + 1;

                std::cout << "Send ACK with SEQ : " << seq << " Expecting SEQ : " << wait_seq << "\n";
                m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
                synFlag = 1;
            }

            else if (dataPkt.type == PKT_TYPE::ACK && dataPkt.seq == 0) {

                std::string peer_ip_port = m_UDPsocket.GetPeerIPandPort(peerInfo);
                std::cout << "Connection Request Received From " << peer_ip_port << "\n";
                ackPkt = Create_ACK_pkt(seq);
                seq = dataPkt.seq;
                wait_seq = seq + 1;

                std::cout << "Send ACK with SEQ : " << seq << " Expecting SEQ : " << wait_seq << "\n";
                m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
                synFlag = 1;
            }

            /************* DATA ************/
            else if (dataPkt.type == PKT_TYPE::DATA && dataPkt.seq > 0 && synFlag == 1) {

                if (dataPkt.seq == wait_seq) {
                    wait_seq++;
                    seq = dataPkt.seq;

                    ackPkt = Create_DATA_ACK_pkt(seq);
                    std::cout << "Received data : " << dataPkt.data << "\n";
                    std::cout << "send ACK with seq  " << seq << " expecting seq : " << wait_seq << "\n";
                    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);

                }
                else {
                    std::cout << "out of order seq : " << dataPkt.seq << "\n";
                    std::cout << "send ACK with seq : " << wait_seq - 1 << "\n";
                    ackPkt = Create_DATA_ACK_pkt(wait_seq - 1);
                    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
                }

            }

            /************* FIN ************/
            else if (dataPkt.type == PKT_TYPE::FIN && synFlag == 1) {
                ackPkt = Create_FIN_ACK_pkt(dataPkt.seq);

                if (finFlag == 0) {
                    std::cout << "Sender is terminating with FIN...\n ";
                    std::cout << "Send ACK with seq : " << dataPkt.seq << "\n";
                    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
                }
                else if (finFlag == 1) {
                    std::cout << "Sender is terminating with FIN... \n";
                    std::cout << "sending ACK with SEQ : " << dataPkt.seq << "\n";
                    m_UDPsocket.SendTo(reinterpret_cast<std::byte*>(&ackPkt), sizeof(ackPkt), peer);
                }

                finFlag = 1;
            }
        }
    }

    // Stop measuring time
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;

    // Output elapsed time
    std::cout << "Total time taken: " << elapsed_seconds.count() << " seconds\n";
}
