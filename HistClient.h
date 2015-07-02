#pragma once

#include "HistoricalData.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <exception>
#include <string>

class HistClient
{
public:
	HistClient();
	historical_data_reply_t get_historical_data(const historical_data_request_t &);
	std::string stringfy(const historical_data_reply_t &);

private:
	static const char* const request_queue_name;
	static const char* const response_queue_name;

	static boost::mutex m;

	boost::interprocess::message_queue request_queue;
	boost::interprocess::message_queue response_queue;
	
	void write_request_message(const historical_data_request_t &);
	historical_data_reply_t read_response_message();
};