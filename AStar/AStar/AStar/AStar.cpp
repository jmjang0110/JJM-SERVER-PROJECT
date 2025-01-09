#include "pch.h"
#include "AStar.h"
#include "Object.h"
#include "GameMap.h"

bool AStar::Update()
{
	int candidate = 0;
	int calcWeight = 0;

	// �켱���� ť : Fcost ( g + h )�� ���� Object ���� Ž�� 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( �̹� Ž���� ��ġ )
	std::unordered_map<int, bool> closeList;

	// ���۳�� �ʱ�ȭ
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

		// ����
		if (pos == m_End.GetPosition()) {

			ReconstructPath(curr);
			ConstructOptimalPath();

			return true;
		}

		// ���� ��� Ž�� (8����)
		// �� �� ��
		//  �� �� ��	
		// �� �� ��
		for (int i = 0; i < 8; ++i) {

			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList[Hash(ny, nx)]) continue;

			Object neighbor(ny, nx);
			float gCost = (i < 4) ? 1 : 1.4f; // �����¿�� 1, �밢���� 1.4

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

	// �켱���� ť : Fcost ( g + h )�� ���� Object ���� Ž�� 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( �̹� Ž���� ��ġ )
	std::unordered_map<int, Object> closeList;

	// ���۳�� �ʱ�ȭ
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

		// ����
		if (pos == m_End.GetPosition()) {
			ReconstructPath(curr);
			return true;
		}

		// ���� ��� Ž�� (8����)
		// �� �� ��
		//  �� �� ��	
		// �� �� ��
		for (int i = 0; i < 8; ++i) {
			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList.find(Hash(ny, nx)) != closeList.end()) continue;

			Object neighbor(ny, nx);
			GameMap[ny][nx] = VISIT;
			float gCost = (i < 4) ? 1 : 1.4f; // �����¿�� 1, �밢���� 1.4

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

	// �켱���� ť : Fcost ( g + h )�� ���� Object ���� Ž�� 
	std::priority_queue<Object, std::vector<Object>, std::greater<Object>> openList;

	// closeList ( �̹� Ž���� ��ġ )
	std::unordered_map<int, Object> closeList;

	// ���۳�� �ʱ�ȭ
	m_Start.SetGCost(0);
	m_Start.SetHCost(Heuristic(m_Start.GetPosition().y, m_Start.GetPosition().x));
	openList.push(m_Start);
	closeList.insert({ Hash(m_Start.GetPosition().y, m_Start.GetPosition().x), m_Start });

	candidate++;

	while (!openList.empty()) {
		Object curr = openList.top();
		openList.pop();
		POSITION pos = curr.GetPosition();

		// ����
		if (pos == m_End.GetPosition()) {
			ReconstructPath(curr);
			return true;
		}

		// ���� ��� Ž�� (8����)
		for (int i = 0; i < 8; ++i) {
			int ny = pos.y + dy[i];
			int nx = pos.x + dx[i];

			if (ny < 0 || ny >= HEIGHT || nx < 0 || nx >= WIDTH) continue;
			else if (GameMap[ny][nx] == OBSTACLE) continue;
			else if (closeList.find(Hash(ny, nx)) != closeList.end()) continue;

			Object neighbor(ny, nx);
			float gCost = (i < 4) ? 1 : 1.4f; // �����¿�� 1, �밢���� 1.4

			neighbor.SetGCost(curr.GetGCost() + gCost);
			neighbor.SetHCost(Heuristic(ny, nx));

			// neighbor�� openList�� ���� ��, �� ���� ��η� G���� ����
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
	path.clear(); // ���� ��� �ʱ�ȭ
	Object temp = current;

	while (temp.GetPosition() != m_Start.GetPosition()) {
		path.push_back(temp);
		temp = parent[temp.GetPosition()];
	}
	path.push_back(m_Start);

	// ��θ� �������� ���� (���������� ����������)
	std::reverse(path.begin(), path.end());

}

void AStar::ConstructOptimalPath()
{
	if (path.size() == 0)
		return;

	optimal_path.push_back(path[0]); // �������� �߰�

	int s = 0;
	for (int i = 1; i < path.size(); ++i) {
		int e = i;

		POSITION start = path[s].GetPosition();
		POSITION end = path[e].GetPosition();

		if (CheckLine(start.x, start.y, end.x, end.y, 40)) {
			// ��ֹ��� ������ ��ο� ��� �߰�
			continue;
		}
		else {
			// ��ֹ��� ������ ���������� ��θ� ����ȭ�� ��ο� �߰�
			optimal_path.push_back(path[e - 1]); // ��ֹ� �������� ��� �߰�
			s = e - 1; // ��ֹ��� �ִ� ������ �ٽ� ����
		}
	}

	// ������ ��ġ �߰�
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
	return y * WIDTH + x; // 2D -> ���������� ��ȯ 
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
