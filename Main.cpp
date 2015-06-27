#pragma once

//#include "HistClient.h"
#include "ProxyServer.h"
#include <iostream>

int main(int argc, char *argv[])
{
	int port = 902;

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	ProxyServer proxy(port);
	proxy.start();

	//HistClient histClient;
	//
	//historical_data_request_t request;
	//request.id = 597387;
	//request.num_samples = 2;

	//historical_data_reply_t reply;

	//try
	//{
	//	reply = histClient.get_historical_data(request);
	//}
	//catch (std::exception &e)
	//{
	//	std::cout << e.what() << std::endl;
	//}

	//system("PAUSE");
	//while (true)
	//{
		// criar o socket.

		// aguardar conexão.

		// criar thread para processar a requisição.

		// ** cada thread vai fazer seu próprio parser da requisição.
		// ** uma thread pro cliente (criar um dicionário que armazenará a thread e as requisições que ela tem que responder). QUANDO AS THREADS DEVERÃO SER MORTAS (x minutos sem requisição)? 
		// ** armazenar as requisições de cada thread em uma fila
	//}
}