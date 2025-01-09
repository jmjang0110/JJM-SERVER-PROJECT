#include "pch.h"
#include "AStar.h"
#include "Object.h"
#include "GameMap.h"

bool AStar::Update()
{
	int candidate = 0;
	int calcWeight = 0;

	// 우선순위 큐 : Fcost ( g + h )가 작은 Object 부터 탐색 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( 이미 탐색된 위치 )
	std::unordered_map<int, bool> closeList;

	// 시작노드 초기화
	m_Start.SetGCost(0);
	m_Start.SetHCost(Heuristic(m_Start.GetPosition().y, m_Start.GetPosition().x));
	openList.push(m_Start);
	candidate++;

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
			ConstructOptimalPath();

			return true;
		}

		// 인접 노드 탐색 (8방향)
		// ↖ ↑ ↗
		//  ← ● →	
		// ↙ ↓ ↘
		for (int i = 0; i < 8; ++i) {

			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList[Hash(ny, nx)]) continue;

			Object neighbor(ny, nx);
			float gCost = (i < 4) ? 1 : 1.4f; // 상하좌우는 1, 대각선은 1.4

			neighbor.SetGCost(curr.GetGCost() + gCost);
			neighbor.SetHCost(Heuristic(ny, nx));

			parent[neighbor.GetPosition()] = curr;
			GameMap[ny][nx] = VISIT;

			openList.push(neighbor);
			candidate++;
			calcWeight++;

		}

		std::cout << " --- \n";
		std::cout << "Candidate : " << candidate << "\n";
		std::cout << "Calc Weight : " << calcWeight << "\n";
	}

	return false;
}

bool AStar::Update2()
{
	int candidate = 0;
	int calcWeight = 0;

	// 우선순위 큐 : Fcost ( g + h )가 작은 Object 부터 탐색 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( 이미 탐색된 위치 )
	std::unordered_map<int, Object> closeList;

	// 시작노드 초기화
	m_Start.SetGCost(0);
	m_Start.SetHCost(Heuristic(m_Start.GetPosition().y, m_Start.GetPosition().x));
	openList.push(m_Start);
	closeList.insert({ Hash(m_Start.GetPosition().y, m_Start.GetPosition().x), m_Start });

	calcWeight++;
	candidate++;

	while (!openList.empty()) {
		Object curr = openList.top();
		openList.pop();
		POSITION pos = curr.GetPosition();

		// 종료
		if (pos == m_End.GetPosition()) {
			ReconstructPath(curr);
			return true;
		}

		// 인접 노드 탐색 (8방향)
		// ↖ ↑ ↗
		//  ← ● →	
		// ↙ ↓ ↘
		for (int i = 0; i < 8; ++i) {
			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList.find(Hash(ny, nx)) != closeList.end()) continue;

			Object neighbor(ny, nx);
			GameMap[ny][nx] = VISIT;
			float gCost = (i < 4) ? 1 : 1.4f; // 상하좌우는 1, 대각선은 1.4

			neighbor.SetGCost(curr.GetGCost() + gCost);
			neighbor.SetHCost(Heuristic(ny, nx));

			if (neighbor.GetFCost() <= curr.GetFCost()) {
				parent[neighbor.GetPosition()] = curr;
				openList.push(neighbor);
				closeList.insert({ Hash(ny, nx), neighbor });
			}

			candidate++;
			calcWeight++;

		}

		std::cout << " --- \n";
		std::cout << "Candidate : " << candidate << "\n";
		std::cout << "Calc Weight : " << calcWeight << "\n";
	}

	return false;
}


bool AStar::Update3() {

	int candidate = 0;
	int calcWeight = 0;

	// 우선순위 큐 : Fcost ( g + h )가 작은 Object 부터 탐색 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( 이미 탐색된 위치 )
	std::unordered_map<int, Object> closeList;

	// 시작노드 초기화
	m_Start.SetGCost(0);
	m_Start.SetHCost(Heuristic(m_Start.GetPosition().y, m_Start.GetPosition().x));
	openList.push(m_Start);
	closeList.insert({ Hash(m_Start.GetPosition().y, m_Start.GetPosition().x), m_Start });

	candidate++;

	while (!openList.empty()) {
		Object curr = openList.top();
		openList.pop();
		POSITION pos = curr.GetPosition();

		// 종료
		if (pos == m_End.GetPosition()) {
			ReconstructPath(curr);
			return true;
		}

		// 인접 노드 탐색 (8방향)
		for (int i = 0; i < 8; ++i) {
			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList.find(Hash(ny, nx)) != closeList.end()) continue;

			Object neighbor(ny, nx);
			float gCost = (i < 4) ? 1 : 1.4f; // 상하좌우는 1, 대각선은 1.4

			neighbor.SetGCost(curr.GetGCost() + gCost);
			neighbor.SetHCost(Heuristic(ny, nx));

			// neighbor가 openList에 있을 때, 더 나은 경로로 G값을 갱신
			if (closeList.find(Hash(ny, nx)) == closeList.end() || neighbor.GetGCost() <= curr.GetGCost()) {
				parent[neighbor.GetPosition()] = curr;
				openList.push(neighbor);
				closeList.insert({ Hash(ny, nx), neighbor });
			}
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

void AStar::ConstructOptimalPath()
{
	if (path.size() == 0)
		return;

	optimal_path.push_back(path[0]); // 시작점을 추가

	int s = 0;
	for (int i = 1; i < path.size(); ++i) {
		int e = i;

		POSITION start = path[s].GetPosition();
		POSITION end = path[e].GetPosition();

		if (CheckLine(start.x, start.y, end.x, end.y, 40)) {
			// 장애물이 없으면 경로에 계속 추가
			continue;
		}
		else {
			// 장애물이 있으면 이전까지의 경로를 최적화된 경로에 추가
			optimal_path.push_back(path[e - 1]); // 장애물 전까지의 경로 추가
			s = e - 1; // 장애물이 있는 곳에서 다시 시작
		}
	}

	// 마지막 위치 추가
	optimal_path.push_back(path[path.size() - 1]);
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

std::vector<Object>& AStar::GetOptimalPath()
{
	return optimal_path;
}
