#include "Packet.h"

#include <iostream>
#include <string>

dataPacket Create_SYN_pkt(int seq_no) {
    dataPacket pkt;

    // SYN 패킷의 type 값은 010 (2)로 설정
    pkt.type = PKT_TYPE::SYN; //0b010;
    pkt.seq = seq_no;

    pkt.data = 0; // data는 SYN 패킷에서 사용되지 않으므로 0으로 설정

    return pkt;
}

dataPacket Create_DATA_pkt(int seq_no, long data) {
    dataPacket pkt;

    // DATA 패킷의 type 값은 000 (0)으로 설정
    pkt.type = PKT_TYPE::DATA; //0b000;
    pkt.seq = seq_no;

    pkt.data = data; // data 값 설정

    return pkt;
}

dataPacket Create_FIN_pkt(int seq_no) {
    dataPacket pkt;

    // FIN 패킷의 type 값은 001 (1)로 설정
    pkt.type = PKT_TYPE::FIN; //0b001;
    pkt.seq = seq_no;

    pkt.data = 0; // data는 FIN 패킷에서 사용되지 않으므로 0으로 설정

    return pkt;
}

ackPacket Create_ACK_pkt(int seq_no)
{
    ackPacket pkt;

    // ACK 패킷의 type 값은 010 (2)로 설정
    pkt.type = PKT_TYPE::ACK;
    pkt.seq = seq_no;

    return pkt;
}

ackPacket Create_SYN_ACK_pkt(int seq_no)
{
    ackPacket pkt;

    pkt.type = PKT_TYPE::SYN_ACK;
    pkt.seq = seq_no;

    return pkt;
}

ackPacket Create_DATA_ACK_pkt(int seq_no)
{
    ackPacket pkt;

    pkt.type = PKT_TYPE::ACK;
    pkt.seq = seq_no;

    return pkt;
}

ackPacket Create_FIN_ACK_pkt(int seq_no)
{
    ackPacket pkt;

    pkt.type = PKT_TYPE::ACK_FIN;
    pkt.seq = seq_no;


    return pkt;
}

UDPholePunchingPacket Create_UDPhpc_Pkt(sockaddr_in peer, char ack)
{
    UDPholePunchingPacket pkt;

    pkt.type = PKT_TYPE::HOLE_PUNCING;
    pkt.seq = 0;

    pkt.peer_end_point = peer;
    pkt.get_peer_success_Ack = ack;

    return pkt;
}

P2PackPacket Create_P2Pack_Pkt()
{
    P2PackPacket pkt;

    pkt.type = PKT_TYPE::P2P_ACK;
    pkt.seq = 0;

    return pkt;
}

P2PDataPacket CreateP2Pdata_Pkt(char str[20])
{
    P2PDataPacket pkt;

    pkt.type = PKT_TYPE::P2P_DATA;
    pkt.seq = 0;

    std::memcpy(pkt.data, str, 20);

    return pkt;
}
