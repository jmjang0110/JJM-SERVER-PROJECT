#include "LockTest.h"
#include "DeadlockDetector.h"

void Lock::lock()
{
#ifdef	_DEBUG
	g_DeadlockDetector.Push(m_name);
#endif


	m_lock.lock();
}

void Lock::unlock()
{
#ifdef _DEBUG
	g_DeadlockDetector.pop(m_name);
#endif


	m_lock.unlock();
}
