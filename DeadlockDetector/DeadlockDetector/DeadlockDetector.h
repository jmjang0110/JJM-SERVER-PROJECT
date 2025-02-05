#pragma once
#include <unordered_map>
#include <set>
#include <mutex>
#include <stack>

struct DDetector {
	std::vector<int>  visitOrder;
	int				  visitOrderCnt;
	std::vector<int>  parents;
	std::vector<bool> finished;
};

class DeadlockDetector
{
private:
	std::unordered_map<std::string, int>   m_LockTable;
	std::unordered_map<int, std::string>   m_LockTableRev;
	std::unordered_map<int, std::set<int>> m_LockEdgeList; // LockID에 포함된 List
	DDetector m_detector;

	std::mutex m_Lock;

	void CheckDeadLock();
	void DFS(int curr);
	bool CheckCycleRoute(int from, int to);

public:
	void Push(const std::string& lock_id);
	void pop(const std::string& lock_id);

};

static DeadlockDetector g_DeadlockDetector;


