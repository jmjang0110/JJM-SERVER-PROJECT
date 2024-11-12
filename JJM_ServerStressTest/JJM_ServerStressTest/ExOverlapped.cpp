#include "pch.h"
#include "ExOverlapped.h"

void ExOverlapped::Clear()
{
	memset(&m_Over, 0, sizeof(m_Over));
	memset(m_Buffer, 0, BUF_SIZE);
}
