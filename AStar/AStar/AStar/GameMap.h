#pragma once

#include <iostream>

#define V2
#ifdef V1
constexpr int WIDTH = 14;
constexpr int HEIGHT = 7;

#elif defined(V2)
constexpr int WIDTH = 20;
constexpr int HEIGHT = 20;

#endif

constexpr int OBSTACLE = 1;
constexpr int ROAD     = 0;
constexpr int VISIT	   = 2;

constexpr int START    = 3;
constexpr int END      = 4;

extern int GameMap[HEIGHT][WIDTH];  // 외부 변수 선언

void PrintMap();  // 함수 선언
void ClearMap();
bool CheckLine(int x1, int y1, int x2, int y2, int cellsize);
