#include "LockTest.h"
#include "DeadlockDetector.h"

void Lock::lock()
{
	g_DeadlockDetector.Push(m_name);


	m_lock.lock();
}

void Lock::unlock()
{
	g_DeadlockDetector.pop(m_name);

	m_lock.unlock();
}
