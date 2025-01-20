#pragma once
#include <unordered_map>
#include <set>
#include <mutex>
#include <stack>


class DeadlockDetector
{
private:
	std::unordered_map<std::string, int> m_LockTable;
	std::unordered_map<int, std::string> m_LockTableRev;
	std::unordered_map<int, std::set<int>> m_LockEdgeList; // LockID에 포함된 List

	std::mutex m_Lock;

	void CheckDeadLock();
	void DFS(int curr);

private:
	std::vector<int>  m_visitOrder;
	int				  m_visitOrderCnt;
	std::vector<int>  m_parents;
	std::vector<bool> m_finished;


public:
	void Push(const std::string& lock_id);
	void pop(const std::string& lock_id);

};

static DeadlockDetector g_DeadlockDetector;


