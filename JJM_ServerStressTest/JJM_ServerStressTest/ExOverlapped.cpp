#include "pch.h"
#include "ExOverlapped.h"

void ExOverlapped::Clear()
{
	// m_Over 구조체 초기화
	ZeroMemory(&m_Over, sizeof(m_Over));	
	memset(m_Buffer, 0, BUF_SIZE);

	m_IOtype = IO_TYPE::READ;
	m_WSABuf.buf = reinterpret_cast<char*>(m_Buffer);
	m_WSABuf.len = sizeof(m_Buffer);
}
