#pragma once

#include "HistClient.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <string>

static const unsigned MAX_LENGTH = 100;
static const unsigned POOL_SIZE = 15;

class ProxyServer
{
public:
	ProxyServer(const int &);
	~ProxyServer();
	void start();

private:
	unsigned available_connections;
	boost::asio::io_service tcp_service;
	boost::asio::io_service worker_service;
	boost::asio::ip::tcp::acceptor tcp_acceptor;
	boost::asio::io_service::work worker;
	boost::condition_variable cv;
	boost::mutex m;
	boost::thread_group threadpool;
	HistClient hist_client;

	void session(boost::asio::ip::tcp::socket *);
	//std::string read_request(boost::asio::ip::tcp::socket *);
	std::string read_request(boost::asio::ip::tcp::socket *);
	//void process_request(boost::asio::ip::tcp::socket *, std::string);
	void process_request(boost::asio::ip::tcp::socket *, const std::string &);
	//void get_active_clients(boost::asio::ip::tcp::socket *);
	std::string get_active_clients();
	//void get_historical_data(const std::string, boost::asio::ip::tcp::socket *);
	std::string get_historical_data(const std::string &);
	void start_threadpool();
	void check_connections_pool();
};

