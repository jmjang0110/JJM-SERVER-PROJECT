#pragma once

/*
Define packet bit structure and type

ACK  SYN  FIN  SEQ  DATA
0    1    0    0    --   --> �ڵ����ŷ�� ���� ù ��° ��Ŷ:			 type = 010 (2)
1    1    0    0    --   --> �������� ACK, ���� ����:					 type = 110 (6)
0    0    0    int  long --> ������ ������ �����ϴ� ��Ŷ:				 type = 000 (0)
1    0    0    int  --   --> ������ ���� �����ڷκ����� ACK:			 type = 100 (4)
0    0    1    int  --   --> ������ ���� �Ϸ�, FIN�� ���Ե� ��Ŷ ����:  type = 001 (1)
1    0    1    int  --   --> ����� ACK ��Ŷ, ���� ����:				 type = 101 (5)

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
    char type;   // ��Ŷ�� Ÿ�� (��: SYN, ACK ��)
    int seq;     // ������ ��ȣ
    long data;   // ������
};

struct ackPacket {
    char type;   // ACK ��Ŷ�� Ÿ��
    int seq;     // ACK�� ������ ��ȣ
};

dataPacket Create_SYN_pkt(int seq_no);
dataPacket Create_DATA_pkt(int seq_no, long data);
dataPacket Create_FIN_pkt(int seq_no);

ackPacket Create_SYN_ACK_pkt(int seq_no);
ackPacket Create_DATA_ACK_pkt(int seq_no);
ackPacket Create_FIN_ACK_pkt(int seq_no);
