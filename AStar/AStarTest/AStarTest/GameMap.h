#pragma once

#include <iostream>

constexpr int WIDTH = 20;
constexpr int HEIGHT = 20;

constexpr int OBSTACLE = 1;
constexpr int ROAD     = 0;
constexpr int START    = 2;
constexpr int END      = 3;

extern int GameMap[HEIGHT][WIDTH];  // �ܺ� ���� ����

void PrintMap();  // �Լ� ����
void ClearMap();