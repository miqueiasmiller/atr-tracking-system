#include "ProxyServer.h"
#include "boost/algorithm/string.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include <iostream>

ProxyServer::ProxyServer(const int port) :
service(),
acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	std::cout << "Application Server - Listening on " << acceptor.local_endpoint() << std::endl;
}

ProxyServer::~ProxyServer()
{
	acceptor.close();
}

void ProxyServer::start()
{
	while (true)
	{
		boost::asio::ip::tcp::socket* s = new boost::asio::ip::tcp::socket(service);
		acceptor.accept(*s);

		std::cout << "Application Server - New conection from " << s->remote_endpoint() << std::endl;

		std::string message = read_request(s);

		process_request(s, message);


		//delete sock;
		//std::cout << "Connection finished" << endl;
		//		std::thread t(session,sock);
		//		t.detach();
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
}

void ProxyServer::process_request(boost::asio::ip::tcp::socket *socket, std::string message)
{
	boost::algorithm::trim(message);

	if (message == "DISCONNECTED")
	{
		delete socket;
		return;
	}

	if (message == "REQ_ATIVOS")
	{
		boost::thread t(boost::bind(&ProxyServer::get_active_clients, this, _1), socket);
		t.detach();
	}
	else if ("")
	{
		delete socket;
	}
}

void ProxyServer::get_active_clients(boost::asio::ip::tcp::socket *socket)
{

	delete socket;
}