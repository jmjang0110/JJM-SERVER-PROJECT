#pragma once

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

dataPacket Create_SYN_pkt(int seq_no);
dataPacket Create_DATA_pkt(int seq_no, long data);
dataPacket Create_FIN_pkt(int seq_no);

ackPacket Create_SYN_ACK_pkt(int seq_no);
ackPacket Create_DATA_ACK_pkt(int seq_no);
ackPacket Create_FIN_ACK_pkt(int seq_no);
