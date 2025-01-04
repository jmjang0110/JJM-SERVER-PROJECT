#include "pch.h"
#include "Client.h"

Client client;

int main(void) 
{

	if (client.Init()) {
		client.Execute();
	}
	client.Exit();

	return 0;
}
