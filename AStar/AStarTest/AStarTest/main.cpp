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

        // 시작 위치와 종료 위치 찾기
        Object start(sy, sx);
        Object end(ey, ex);

        // A* 객체 생성
        AStar astar;
        astar.SetStart(start);
        astar.SetEnd(end);

        // 탐색 실행
        if (astar.Update()) {
            std::cout << "경로를 찾았습니다:\n";

            // 탐색된 경로 표시
            for (auto& node : astar.GetPath()) {
                POSITION pos = node.GetPosition();
                GameMap[pos.y][pos.x] = '*'; // '*'로 경로 표시
            }
        }
        else {
            std::cout << "경로를 찾을 수 없습니다.\n";
        }

        // 맵 출력
        PrintMap();
        int a;
        std::cin >> a;
    }
    

    return 0;
}
