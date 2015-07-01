#include "ProxyServer.h"

ProxyServer::ProxyServer(const int port) :
	tcp_service(),
	worker_service(),
	tcp_acceptor(tcp_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	worker(worker_service),
	historiador()
{
	start_threadpool();

	std::cout << "Application Server - Listening on " << tcp_acceptor.local_endpoint() << std::endl;
}


ProxyServer::~ProxyServer()
{
	tcp_service.stop();

	tcp_acceptor.close();

	// para o loop serviço para que nenhuma tarefa após esse ponto seja executada.
	worker_service.stop();

	// aguarda o fim das tarefas em execução
	threadpool.join_all();
}


void ProxyServer::start_threadpool()
{
	std::cout << "Application Server - Start - Loading Thread Pool - Pool size = " << POOL_SIZE << std::endl;

	for (unsigned i = 0; i < POOL_SIZE; i++)
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &worker_service));

	std::cout << "Application Server - End - Loading Thread Pool" << std::endl;
}


void ProxyServer::start()
{
	while (true)
	{
		boost::asio::ip::tcp::socket* s = new boost::asio::ip::tcp::socket(tcp_service);
		tcp_acceptor.accept(*s);

		std::cout << "Application Server - New conection from " << s->remote_endpoint() << std::endl;

		std::string message = read_request(s);

		process_request(s, message);
	}
}


std::string ProxyServer::read_request(boost::asio::ip::tcp::socket * socket)
{
	try
	{
		char data[MAX_LENGTH];

		boost::system::error_code error;

		size_t length = socket->read_some(boost::asio::buffer(data, MAX_LENGTH), error);

		if (error == boost::asio::error::eof) // Client has disconnected
			return "DISCONNECTED";

		if (error)
			throw boost::system::system_error(error); // Some other error.

		std::cout << "Application Server - Message received from " << socket->remote_endpoint() << " - Message: " << data << std::endl;

		return std::string(data);
	}
	catch (std::exception &e)
	{
		std::cerr << "Application Server - process_request - " << e.what() << std::endl;
	}

	return "ERROR";
}


void ProxyServer::process_request(boost::asio::ip::tcp::socket *socket, std::string message)
{
	boost::algorithm::trim(message);

	if (message == "DISCONNECTED")
	{
		delete socket;
	}
	else if (message == "REQ_ATIVOS")
	{
		worker_service.post(boost::bind(&ProxyServer::get_active_clients, this, socket));
	}
	else if (message.substr(0, 8) == "REQ_HIST") //REQ_HIST;<ID>;<NUM_AMOSTRAS>
	{
		worker_service.post(boost::bind(&ProxyServer::get_historical_data, this, message, socket));
	}
	else
	{
		delete socket;
	}
}


void ProxyServer::get_active_clients(boost::asio::ip::tcp::socket *socket)
{
	boost::this_thread::sleep(boost::posix_time::millisec(1000));

	std::cout << "Connection finished - " << socket->remote_endpoint() << std::endl;
	delete socket;
}


void ProxyServer::get_historical_data(const std::string message, boost::asio::ip::tcp::socket *socket)
{
	std::vector<std::string> tokens;
	boost::split(tokens, message, boost::is_any_of(";"));

	historical_data_reply_t reply;

	if (tokens.size() == 3)
	{
		historical_data_request_t request;
		request.id = std::stoi(tokens.at(1));
		request.num_samples = std::stoi(tokens.at(2));

		reply = historiador.get_historical_data(request);
	}

	std::cout << "Connection finished - " << socket->remote_endpoint() << std::endl;
	delete socket;
}