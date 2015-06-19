#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <exception>
#include <string>
#include "HistoricalData.h"

class HistClient
{
public:
	HistClient();
	~HistClient();
	historical_data_reply_t get_historical_data(historical_data_request_t const&);
private:
	static const char* const request_queue_name;
	static const char* const response_queue_name;

	static boost::mutex rwMutex;

	boost::interprocess::message_queue request_queue;
	boost::interprocess::message_queue response_queue;
	
	void write_request_message(historical_data_request_t const&);
	historical_data_reply_t read_response_message();
};