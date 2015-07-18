#include "HistClient.h"

char const * const HistClient::request_queue_name = "servapp_historiador";
char const * const HistClient::response_queue_name = "historiador_servapp";
boost::mutex HistClient::m;

HistClient::HistClient() :
	request_queue(boost::interprocess::open_only, request_queue_name),
	response_queue(boost::interprocess::open_only, response_queue_name)
{
	std::cout << "Application Server - Request Queue opened." << std::endl;
	std::cout << "Application Server - Response Queue opened." << std::endl;
}


historical_data_reply_t HistClient::get_historical_data(historical_data_request_t const & request)
{
	boost::lock_guard<boost::mutex> lock(m);

	write_request_message(request);

	return read_response_message();
}


void HistClient::write_request_message(historical_data_request_t const & request)
{
	if (request.num_samples > 1 && request.num_samples <= MAX_POSITION_SAMPLES)
	{
		// envia a mensagem (se o buffer estiver cheio, a thread fica bloqueada até que haja espaço).
		request_queue.send(&request, sizeof(request), 0);
	}
	else if (request.num_samples == 1)
	{
		// requisita do gateway via memória compartilhada.
		throw std::exception("não implementado.");
	}
	else
	{
		std::string const msg = "O número de amostras requisitadas não é válido. Este deve estar entre 1 e " + std::to_string(MAX_POSITION_SAMPLES) + ".";
		throw std::exception(msg.c_str());
	}
}


historical_data_reply_t HistClient::read_response_message()
{
	historical_data_reply_t reply;
	boost::interprocess::message_queue::size_type received_size;
	unsigned int priority;

	// recebe a mensagem (se não existe mensage, a thread fica bloqueada).
	response_queue.receive(&reply, sizeof(historical_data_reply_t), received_size, priority);

	if (received_size != sizeof(reply))
	{
		std::string msg = "A mensagem retornada pelo historiador é inválida.";
		throw std::exception(msg.c_str());
	}

	return reply;
}

std::string HistClient::stringfy(historical_data_reply_t const & reply)
{
	if (reply.num_samples_available == 0)
	{
		return "HIST;0";
	}
	else
	{
		std::stringstream ss;
		
		ss << "HIST;" << reply.num_samples_available;
		ss << ";" << reply.data[0].header.id;

		for (int i = 0; i < reply.num_samples_available; i++)
		{
			ss << ";POS";
			ss << ";" << std::string(reply.data[i].data.timestamp, 14);
			ss << ";" << boost::lexical_cast<std::string>(reply.data[i].data.latitude);
			ss << ";" << boost::lexical_cast<std::string>(reply.data[i].data.longitude);
			ss << ";" << reply.data[i].data.speed;
			ss << ";" << reply.data[i].data.status ? "1" : "0";
		}

		return ss.str();
	}
}