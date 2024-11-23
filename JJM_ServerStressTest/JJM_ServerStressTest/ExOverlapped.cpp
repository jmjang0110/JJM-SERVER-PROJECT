#include "pch.h"
#include "ExOverlapped.h"

void ExOverlapped::Clear()
{
	memset(&m_Over, 0, sizeof(m_Over));
	memset(m_Buffer, 0, BUF_SIZE);

	m_IOtype = IO_TYPE::READ;
	m_WSABuf.buf = reinterpret_cast<char*>(m_Buffer);
	m_WSABuf.len = sizeof(m_Buffer);
}
