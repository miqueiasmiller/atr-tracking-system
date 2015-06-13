#include "HistClient.h"

HistClient::HistClient() : 
	request_queue(boost::interprocess::open_only, request_queue_name.c_str()),
	response_queue(boost::interprocess::open_only, response_queue_name.c_str())
{
}


HistClient::~HistClient()
{
	boost::interprocess::message_queue::remove(request_queue_name.c_str());
	boost::interprocess::message_queue::remove(response_queue_name.c_str());
}


historical_data_reply_t HistClient::get_historical_data(historical_data_request_t const& request)
{
	boost::unique_lock<boost::mutex> lock(rwMutex);

	write_request_message(request);

	return read_response_message();
}


void HistClient::write_request_message(historical_data_request_t const& request)
{
	if (request.num_samples > MAX_POSITION_SAMPLES)
	{
		std::string const msg = "O número de amostras requisitadas é maior do que o máximo permitido [" + std::to_string(MAX_POSITION_SAMPLES) + "].";
		throw std::exception(msg.c_str());
	}
	else if (request.num_samples == 1)
	{
		// requisita do gateway via memória compartilhada.
		throw std::exception("não implementado.");
	}

	// envia a mensagem (se o buffer estiver cheio, a thread fica bloqueada até que aja espaço).
	request_queue.send(&request, sizeof(request), 0);
}


historical_data_reply_t HistClient::read_response_message()
{
	historical_data_reply_t reply;
	boost::interprocess::message_queue::size_type received_size;
	unsigned int priority;

	// recebe a mensagem (se não existe mensage, a thread fica bloqueada).
	response_queue.receive(&reply, sizeof(reply), received_size, priority);

	if (received_size != sizeof(reply))
	{
		std::string msg = "A mensagem retornado pelo historiador é inválida.";
		throw std::exception(msg.c_str());
	}

	return reply;
}