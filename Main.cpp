#pragma once

#include "HistClient.h"

int main(int argc, char *argv[])
{
	HistClient histClient;
	
	historical_data_request_t request;
	request.id = 1;
	request.num_samples = 10;

	historical_data_reply_t reply;

	reply = histClient.get_historical_data(request);


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