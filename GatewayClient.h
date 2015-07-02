#include "HistoricalData.h"
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread.hpp>


//std::string Usuarios_Ativos(shared_memory_buffer &);
//shared_memory_buffer* gateway_client_start();
//void gateway_client_start(shared_memory_buffer *);
//std::string gateway_client_get_data(shared_memory_buffer *);


class GatewayClient
{
public:
	GatewayClient();
	std::string get_ativos();
	historical_data_reply_t get_historico(int const);

private:
	boost::interprocess::shared_memory_object shm;
	boost::interprocess::mapped_region region;
	shared_memory_buffer* data;

	std::string Usuarios_Ativos();
};