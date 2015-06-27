#pragma once

#include "boost/asio.hpp"
#include <string>

static const unsigned MAX_LENGTH = 100;

class ProxyServer
{
public:
	ProxyServer(const int);
	~ProxyServer();
	void start();

private:
	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;
	std::string read_request(boost::asio::ip::tcp::socket *);
	void process_request(boost::asio::ip::tcp::socket *, std::string);
	void get_active_clients(boost::asio::ip::tcp::socket *);
};

