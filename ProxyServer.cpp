#include "ProxyServer.h"

ProxyServer::ProxyServer(const int & port) :
tcp_service(),
worker_service(),
tcp_acceptor(tcp_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
worker(worker_service),
hist_client()
{
	available_connections = POOL_SIZE;

	start_threadpool();

	std::cout << "Application Server - Listening on " << tcp_acceptor.local_endpoint() << std::endl;
}


ProxyServer::~ProxyServer()
{
	// para o loop serviço para que nenhuma tarefa após esse ponto seja executada.
	worker_service.stop();

	tcp_service.stop();

	// aguarda o fim das tarefas em execução
	threadpool.join_all();
}


void ProxyServer::start_threadpool()
{
	std::cout << "Application Server - Loading Thread Pool - Pool size = " << POOL_SIZE << std::endl;

	for (unsigned i = 0; i < POOL_SIZE; i++)
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &worker_service));
}


void ProxyServer::start()
{
	while (true)
	{
		check_connections_pool();

		boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(tcp_service);
		tcp_acceptor.accept(*socket);

		std::cout << "Application Server - New conection from " << socket->remote_endpoint() << std::endl;

		worker_service.post(boost::bind(&ProxyServer::session, this, socket));
	}
}


void ProxyServer::check_connections_pool()
{
	boost::unique_lock<boost::mutex> lock(m);
	while (available_connections <= 0)
	{
		std::cout << "Application Server - Maximun number of connections reached" << std::endl;
		cv.wait(lock);
	}

	--available_connections;
}


void ProxyServer::session(boost::asio::ip::tcp::socket* socket)
{
	try
	{
		while (true)
		{
			std::string message = read_request(socket);

			if (message == "DISCONNECTED")
			{
				std::cerr << "Application Server - Client has disconnected." << socket->remote_endpoint() << std::endl;
				break;
			}

			process_request(socket, message);
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Application Server - session error - " << e.what() << std::endl;
	}

	if (socket->is_open())
		socket->close();

	// libera uma conexão para o pool...
	boost::unique_lock<boost::mutex> lock(m);
	++available_connections;
	cv.notify_one();
}


std::string ProxyServer::read_request(boost::asio::ip::tcp::socket * socket)
{
	boost::system::error_code error;
	char data[MAX_LENGTH];

	size_t length = socket->read_some(boost::asio::buffer(data, MAX_LENGTH), error);

	if (error == boost::asio::error::eof) // Client has disconnected
		return "DISCONNECTED";
	else if (error)
		throw boost::system::system_error(error); // Some other error.

	return std::string(data, length);
}


void ProxyServer::process_request(boost::asio::ip::tcp::socket * socket, const std::string & message)
{
	if (message == "REQ_ATIVOS")
	{
		std::cout << "Application Server - Message received from " << socket->remote_endpoint() << " - Message: " << message << std::endl;
		socket->write_some(boost::asio::buffer(get_active_clients()));
	}
	else if (message.length() >= 8 && message.substr(0, 8) == "REQ_HIST")
	{
		std::cout << "Application Server - Message received from " << socket->remote_endpoint() << " - Message: " << message << std::endl;
		socket->write_some(boost::asio::buffer(get_historical_data(message)));
	}
}


std::string ProxyServer::get_active_clients()
{
	return "ATIVOS;0";
}


std::string ProxyServer::get_historical_data(const std::string & message)
{
	std::vector<std::string> tokens;
	boost::split(tokens, message, boost::is_any_of(";"));

	historical_data_reply_t reply;

	if (tokens.size() == 3)
	{
		historical_data_request_t request;
		request.id = std::stoi(tokens.at(1));
		request.num_samples = std::stoi(tokens.at(2));

		reply = hist_client.get_historical_data(request);

		return hist_client.stringfy(reply);
	}
	else
	{
		return "INVALID REQUEST - " + message;
	}
}