#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib") // Winsock 라이브러리 링크

/*
Define packet bit structure and type

ACK  SYN  FIN  SEQ  DATA
0    1    0    0    --   --> 핸드셰이킹을 위한 첫 번째 패킷:			 type = 010 (2)
1    1    0    0    --   --> 수신자의 ACK, 연결 수립:					 type = 110 (6)
0    0    0    int  long --> 데이터 전송을 시작하는 패킷:				 type = 000 (0)
1    0    0    int  --   --> 데이터 없이 수신자로부터의 ACK:			 type = 100 (4)
0    0    1    int  --   --> 데이터 전송 완료, FIN이 포함된 패킷 전송:  type = 001 (1)
1    0    1    int  --   --> 종료된 ACK 패킷, 연결 종료:				 type = 101 (5)

*/

namespace PKT_TYPE {
    constexpr char DATA    = '0';
    constexpr char FIN     = '1';
    constexpr char SYN     = '2';
    constexpr char ACK     = '4';
    constexpr char ACK_FIN = '5';
    constexpr char SYN_ACK = '6';

    constexpr char HOLE_PUNCING = '7';
    constexpr char P2P_ACK = '8';
    constexpr char P2P_DATA = '9';
};


struct dataPacket {
    char type;   // 패킷의 타입 (예: SYN, ACK 등)
    int seq;     // 시퀀스 번호
    long data;   // 데이터
};

struct ackPacket {
    char type;   // ACK 패킷의 타입
    int seq;     // ACK의 시퀀스 번호
};

struct PacketHeader {
    char type;
    int seq;
};

struct UDPholePunchingPacket : public PacketHeader{
    sockaddr_in peer_end_point;
    char get_peer_success_Ack;
};

struct P2PackPacket : public PacketHeader {

};

struct P2PDataPacket : public PacketHeader {
    char data[20];
};

dataPacket Create_SYN_pkt(int seq_no);
dataPacket Create_DATA_pkt(int seq_no, long data);
dataPacket Create_FIN_pkt(int seq_no);

ackPacket Create_ACK_pkt(int seq_no);
ackPacket Create_SYN_ACK_pkt(int seq_no);
ackPacket Create_DATA_ACK_pkt(int seq_no);
ackPacket Create_FIN_ACK_pkt(int seq_no);

UDPholePunchingPacket Create_UDPhpc_Pkt(sockaddr_in peer, char ack);
P2PackPacket Create_P2Pack_Pkt();
P2PDataPacket CreateP2Pdata_Pkt(char str[20]);
