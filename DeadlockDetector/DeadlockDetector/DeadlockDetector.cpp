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
	if (iter == m_LockEdgeList.end()) { // from��忡 ����� ������ ���� 
		m_finished[from] = false;
		return;
	}

	// from -> to,to,to...
	auto& ToList = iter->second;
	for (int to : ToList) {
		if (m_visitOrder[to] == -1) {
			m_parents[to] = from; // �湮�� ����� �θ�� �̹� ���
			DFS(to);
			continue;
		}

		// ������
		if (m_visitOrder[from] < m_visitOrder[to]) {
			continue;
		}

		// DFS �� �Ϸ���� ���� ������ ��带 �ٽ� �湮�ϸ� ����Ŭ �߻��� �� Dead lock!
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
	// ���ο� id 
	if (m_LockTable.find(lock_name) == m_LockTable.end()) {
		id = m_LockTable.size();
		m_LockTable[lock_name] = id;
		m_LockTableRev[id] = lock_name;

	}
	// �����ϴ� id
	else {
		id = m_LockTable.find(lock_name)->second;
	}

	if (!tls_lockStack.empty()) {
		const int top = tls_lockStack.top();
		if (top != id) {
			// ������ ���� �ȵ� ���ο� ���̸� �˻� 
			if (m_LockEdgeList[top].find(id) == m_LockEdgeList[top].end()) {
				m_LockEdgeList[top].insert(id);
				CheckDeadLock();
			}
		}
		// ���� ���̸� �̹� �������ΰ� �ٽ� ��û�ϴ°Ŷ� ����� �˻���ص���
	}

	tls_lockStack.push(id);
}

void DeadlockDetector::pop(const std::string& lock_id)
{
	std::lock_guard<std::mutex> L(m_Lock);
	tls_lockStack.pop();
}

