#pragma once

#include <mutex>
#include <string>

class Lock {
private:
	std::mutex m_lock;
	std::string m_name;

public:
	Lock() = delete;
	Lock(std::string name) : m_name(name) {}

	void lock();
	void unlock();

};