#include "pch.h"
#include "AStar.h"
#include "Object.h"
#include "GameMap.h"

bool AStar::Update()
{
	// 우선순위 큐 : Fcost ( g + h )가 작은 Object 부터 탐색 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( 이미 탐색된 위치 )
	std::unordered_map<int, bool> closeList;

	// 시작노드 초기화
	m_Start.SetGCost(0);
	m_Start.SetHCost(Heuristic(m_Start.GetPosition().y, m_Start.GetPosition().x));
	openList.push(m_Start);

	while (!openList.empty()) {
		Object curr = openList.top();
		openList.pop();
		POSITION pos = curr.GetPosition();

		if (closeList[Hash(pos.y, pos.x)] == true)
			continue;
		closeList[Hash(pos.y, pos.x)] = true;

		// 종료
		if (pos == m_End.GetPosition()) {
			ReconstructPath(curr);
			return true;
		}

		// 인접 노드 탐색
		// ↖ ↑ ↗
		//  ← ● →	
		// ↙ ↓ ↘
		for (int i = 0; i < 8; ++i) {
			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			if (GameMap[ny][nx] == OBSTACLE) continue;
			if (closeList[Hash(ny, nx)]) continue;

			Object neighbor(ny, nx);
			neighbor.SetGCost(curr.GetGCost() + 1);
			neighbor.SetHCost(Heuristic(ny, nx));
			parent[neighbor.GetPosition()] = curr;

			openList.push(neighbor);
		}


	}

	return false;
}

void AStar::ReconstructPath(const Object& current)
{
	path.clear(); // 기존 경로 초기화
	Object temp = current;

	while (temp.GetPosition() != m_Start.GetPosition()) {
		path.push_back(temp);
		temp = parent[temp.GetPosition()];
	}
	path.push_back(m_Start);

	// 경로를 역순으로 정렬 (시작점부터 종료점으로)
	std::reverse(path.begin(), path.end());
}

int AStar::Heuristic(int y, int x)
{
	int dx = std::abs(x - m_End.GetPosition().x);
	int dy = std::abs(y - m_End.GetPosition().y);
	return dx * dx + dy * dy;
}

int AStar::Hash(int y, int x)
{
	return y * WIDTH + x; // 2D -> 고유값으로 변환 
}

void AStar::SetStart(const Object& start)
{
	m_Start = start;
}

void AStar::SetEnd(const Object& end)
{
	m_End = end;
}

std::vector<Object>& AStar::GetPath()
{
	return path;
}