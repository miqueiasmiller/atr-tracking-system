#pragma once

#include "HistClient.h"
#include "boost/algorithm/string.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include <iostream>
#include <string>

static const unsigned MAX_LENGTH = 100;
static const unsigned POOL_SIZE = 15;

class ProxyServer
{
public:
	ProxyServer(const int);
	~ProxyServer();
	void start();

private:
	boost::asio::io_service tcp_service;
	boost::asio::io_service worker_service;
	boost::asio::ip::tcp::acceptor tcp_acceptor;
	boost::asio::io_service::work worker;
	boost::thread_group threadpool;
	HistClient historiador;

	std::string read_request(boost::asio::ip::tcp::socket *);
	void process_request(boost::asio::ip::tcp::socket *, std::string);
	void get_active_clients(boost::asio::ip::tcp::socket *);
	void get_historical_data(const std::string, boost::asio::ip::tcp::socket *);
	void start_threadpool();
};

