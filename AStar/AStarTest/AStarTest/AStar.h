#pragma once
#include "Object.h"

class Object;
class POSITION;
class AStar
{
private:
	Object m_Start;
	Object m_End;
	std::unordered_map<POSITION, Object> parent;
	std::vector<Object> path;                   // 경로 저장

	// 8방향 이동 (상, 하, 좌, 우, 대각선 4방향)
	int dx[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	int dy[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };

public:

	bool Update();
	int Heuristic(int y, int x);
	int Hash(int y, int x);
	void ReconstructPath(const Object& current);

	void SetStart(const Object& start);
	void SetEnd(const Object& end);
	std::vector<Object>& GetPath();
	
};

