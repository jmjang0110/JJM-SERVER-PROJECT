#pragma once
class Client
{
private:
	RUDPSocket m_UDPsocket;

public:
	bool Init();
	void Exit();
	void Execute();

};

