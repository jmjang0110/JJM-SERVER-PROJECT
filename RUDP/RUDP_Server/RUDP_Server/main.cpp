#include "pch.h"
#include "Server.h"

Server server;

int main(void) 
{
	
	if(server.Init()){
		server.Execute();
	}
	server.Exit();

	return 0;
}
