#include "pch.h"
#include "AStar.h"
#include "Object.h"
#include "GameMap.h"


int main()
{
    while (true) {
        ClearMap();
        PrintMap();

        //system("cls");
        int sy = 5, sx = 1;
        int ey = 1, ex = 12;
        //std::cout << "START : ";
        //std::cin >> sy >> sx;
       // std::cout << "END : ";
        //std::cin >> ey >> ex;

        // ���� ��ġ�� ���� ��ġ ã��
        //Object start(sy, sx);
        //Object end(ey, ex);
        
        Object start(sy, sx);
        Object end(ey, ex);

        // A* ��ü ����
        AStar astar;
        astar.SetStart(start);
        astar.SetEnd(end);

        // Ž�� ����
        if (astar.Update2()) {
            std::cout << "��θ� ã�ҽ��ϴ�:\n";

            // Ž���� ��� ǥ��
            for (auto& node : astar.GetPath()) {
                POSITION pos = node.GetPosition();
                GameMap[pos.y][pos.x] = '*'; // '*'�� ��� ǥ��
                // �� ���
                PrintMap();

            }
        }
        else {
            std::cout << "��θ� ã�� �� �����ϴ�.\n";
        }

        int a;
        std::cin >> a;
    }
    

    return 0;
}
