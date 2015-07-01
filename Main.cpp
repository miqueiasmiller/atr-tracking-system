#pragma once

#include "ProxyServer.h"

void start_server(int port)
{
	ProxyServer proxy(port);
	proxy.start();
}

int main(int argc, char *argv[])
{
	int port = 902;

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	ProxyServer proxy(port);
	proxy.start();

	/*boost::thread t(boost::bind(start_server, port));
	t.join();*/
}