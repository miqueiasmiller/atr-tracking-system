#pragma once

#include "HistoricalData.h"
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>

class GatewayClient
{
public:
	GatewayClient();
	std::string get_ativos();
	historical_data_reply_t get_historico(int const);

private:
	boost::interprocess::shared_memory_object shm;
	boost::interprocess::mapped_region region;
	active_users_t* data;

	std::string Usuarios_Ativos();
};