#include "pch.h"
#include "AStar.h"
#include "Object.h"
#include "GameMap.h"

int main()
{
    while (true) {
        ClearMap();
        PrintMap();

        system("cls");
        int sy, sx;
        int ey, ex;
        std::cout << "START : ";
        std::cin >> sy >> sx;
        std::cout << "END : ";
        std::cin >> ey >> ex;

        // ���� ��ġ�� ���� ��ġ ã��
        Object start(sy, sx);
        Object end(ey, ex);

        // A* ��ü ����
        AStar astar;
        astar.SetStart(start);
        astar.SetEnd(end);

        // Ž�� ����
        if (astar.Update()) {
            std::cout << "��θ� ã�ҽ��ϴ�:\n";

            // Ž���� ��� ǥ��
            for (auto& node : astar.GetPath()) {
                POSITION pos = node.GetPosition();
                GameMap[pos.y][pos.x] = '*'; // '*'�� ��� ǥ��
            }
        }
        else {
            std::cout << "��θ� ã�� �� �����ϴ�.\n";
        }

        // �� ���
        PrintMap();
        int a;
        std::cin >> a;
    }
    

    return 0;
}
