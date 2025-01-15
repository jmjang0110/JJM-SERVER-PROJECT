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

struct dataPacket {
    char type;   // ��Ŷ�� Ÿ�� (��: SYN, ACK ��)
    int seq;     // ������ ��ȣ
    long data;   // ������
};

struct ackPacket {
    char type;   // ACK ��Ŷ�� Ÿ��
    int seq;     // ACK�� ������ ��ȣ
};

dataPacket Create_SYN_pkt(int seq_no) {
    dataPacket pkt;

    // SYN ��Ŷ�� type ���� 010 (2)�� ����
    pkt.type = 0b010;
    pkt.seq = seq_no;

    pkt.data = 0; // data�� SYN ��Ŷ���� ������ �����Ƿ� 0���� ����

    return pkt;
}

dataPacket Create_DATA_pkt(int seq_no, long data) {
    dataPacket pkt;

    // DATA ��Ŷ�� type ���� 000 (0)���� ����
    pkt.type = 0b000;
    pkt.seq = seq_no;

    pkt.data = data; // data �� ����

    return pkt;
}

dataPacket Create_FIN_pkt(int seq_no) {
    dataPacket pkt;

    // FIN ��Ŷ�� type ���� 001 (1)�� ����
    pkt.type = 0b001;
    pkt.seq = seq_no;

    pkt.data = 0; // data�� FIN ��Ŷ���� ������ �����Ƿ� 0���� ����

    return pkt;
}
