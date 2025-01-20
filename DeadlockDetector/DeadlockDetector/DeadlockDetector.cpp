#include "DeadlockDetector.h"
#include <iostream>
#include <cassert>

thread_local std::stack<int> tls_lockStack;

void DeadlockDetector::CheckDeadLock()
{
	const int lock_cnt = m_LockTable.size();

	m_visitOrder    = std::vector<int>(lock_cnt, -1);
	m_parents       = std::vector<int>(lock_cnt, -1);
	m_finished      = std::vector<bool>(lock_cnt, false);
	m_visitOrderCnt = 0;

	for (int i = 0; i < lock_cnt; ++i) {
		DFS(i);
	}


	m_visitOrder.clear();
	m_parents.clear();
	m_finished.clear();

}

void DeadlockDetector::DFS(int from)
{
	if (m_visitOrder[from] != -1)
		return;

	m_visitOrder[from] = m_visitOrderCnt++;
	auto iter = m_LockEdgeList.find(from);
	if (iter == m_LockEdgeList.end()) { // from노드에 연결된 간선이 없음 
		m_finished[from] = false;
		return;
	}

	// from -> to,to,to...
	auto& ToList = iter->second;
	for (int to : ToList) {
		if (m_visitOrder[to] == -1) {
			m_parents[to] = from; // 방문할 노드의 부모는 이번 노드
			DFS(to);
			continue;
		}

		// 순방향
		if (m_visitOrder[from] < m_visitOrder[to]) {
			continue;
		}

		// DFS 중 완료되지 않은 상태의 노드를 다시 방문하면 사이클 발생한 것 Dead lock!
		if (m_finished[to] == false) {
			
			std::cout << "DEAD LOCK!\n";
			std::cout << m_LockTableRev[from].c_str() << " -> " << m_LockTableRev[to].c_str();

			int curr = from;
			while(true){
				int parent = m_parents[curr];
				curr = parent;
				if (curr == to)
					break;
			}

			assert(false, "DEAD LOCK");
		}
	}

	m_finished[from] = true;
}

void DeadlockDetector::Push(const std::string& lock_name)
{
	std::lock_guard<std::mutex> L(m_Lock);

	int id = -1;
	// 새로운 id 
	if (m_LockTable.find(lock_name) == m_LockTable.end()) {
		id = m_LockTable.size();
		m_LockTable[lock_name] = id;
		m_LockTableRev[id] = lock_name;

	}
	// 존재하는 id
	else {
		id = m_LockTable.find(lock_name)->second;
	}

	if (!tls_lockStack.empty()) {
		const int top = tls_lockStack.top();
		if (top != id) {
			// 이전에 참조 안된 새로운 락이면 검사 
			if (m_LockEdgeList[top].find(id) == m_LockEdgeList[top].end()) {
				m_LockEdgeList[top].insert(id);
				CheckDeadLock();
			}
		}
		// 같은 락이면 이미 보유중인걸 다시 요청하는거라 데드락 검사안해도됨
	}

	tls_lockStack.push(id);
}

void DeadlockDetector::pop(const std::string& lock_id)
{
	std::lock_guard<std::mutex> L(m_Lock);
	tls_lockStack.pop();
}

