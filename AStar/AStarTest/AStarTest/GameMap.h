#pragma once

#include <iostream>

constexpr int WIDTH = 20;
constexpr int HEIGHT = 20;

constexpr int OBSTACLE = 1;
constexpr int ROAD     = 0;
constexpr int START    = 2;
constexpr int END      = 3;

extern int GameMap[HEIGHT][WIDTH];  // 외부 변수 선언

void PrintMap();  // 함수 선언
void ClearMap();