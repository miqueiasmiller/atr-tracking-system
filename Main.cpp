#pragma once

#include "ProxyServer.h"

int main(int argc, char *argv[])
{
	int port = 902;

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	ProxyServer proxy(port);
	proxy.start();
}