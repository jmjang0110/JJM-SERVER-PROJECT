#include "Packet.h"

#include <iostream>
#include <string>

dataPacket Create_SYN_pkt(int seq_no) {
    dataPacket pkt;

    // SYN ��Ŷ�� type ���� 010 (2)�� ����
    pkt.type = PKT_TYPE::SYN; //0b010;
    pkt.seq = seq_no;

    pkt.data = 0; // data�� SYN ��Ŷ���� ������ �����Ƿ� 0���� ����

    return pkt;
}

dataPacket Create_DATA_pkt(int seq_no, long data) {
    dataPacket pkt;

    // DATA ��Ŷ�� type ���� 000 (0)���� ����
    pkt.type = PKT_TYPE::DATA; //0b000;
    pkt.seq = seq_no;

    pkt.data = data; // data �� ����

    return pkt;
}

dataPacket Create_FIN_pkt(int seq_no) {
    dataPacket pkt;

    // FIN ��Ŷ�� type ���� 001 (1)�� ����
    pkt.type = PKT_TYPE::FIN; //0b001;
    pkt.seq = seq_no;

    pkt.data = 0; // data�� FIN ��Ŷ���� ������ �����Ƿ� 0���� ����

    return pkt;
}

ackPacket Create_ACK_pkt(int seq_no)
{
    ackPacket pkt;

    // ACK ��Ŷ�� type ���� 010 (2)�� ����
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
