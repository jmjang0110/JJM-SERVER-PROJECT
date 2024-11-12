#pragma once

enum class IO_TYPE{
	NONE,
	READ, WRITE, CONNECT,
	END,
};
constexpr int BUF_SIZE = 1024;

class ExOverlapped
{
public:
	WSAOVERLAPPED	m_Over{};
	WSABUF			m_WSABuf{};
	IO_TYPE			m_IOtype{};
	BYTE			m_Buffer[BUF_SIZE]{};

public:
	void Clear();


};

