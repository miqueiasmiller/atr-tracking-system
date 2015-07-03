#pragma once

#include "GatewayClient.h"

using namespace boost::interprocess;
using namespace std;

GatewayClient::GatewayClient() : 
	shm(open_only, "usuariosAtivos", read_write),
	region(shm, read_write)
{
	data = static_cast<shared_memory_buffer*>(region.get_address());

	cout << "Application Server - Shared Memory opened." << endl;
}


string GatewayClient::Usuarios_Ativos() {

	string str1_id, str2_imei, str3_ativo, aux, str4_num_active;
	int int_id, int_num_active;

	int_num_active = data->num_active_users;
	str4_num_active = to_string(int_num_active);

	aux = ";";

	str3_ativo = "ATIVOS;";
	str3_ativo = str3_ativo + str4_num_active;

	if (data->num_active_users != 0)
	{
		for (int j = 0; j < data->LIST_SIZE; j++)
		{

			if (data->list[j].header.id != -1)
			{
				str3_ativo = str3_ativo + aux;
				int_id = data->list[j].header.id;
				str1_id = to_string(int_id);
				str2_imei = data->list[j].header.imei;
				str3_ativo = str3_ativo + str1_id + aux + str2_imei;
			}
		}
	}
	else
		str3_ativo = "ATIVOS;0";

	//cout << str3_ativo << endl;
	return str3_ativo;
}


string GatewayClient::get_ativos()
{
	boost::mutex m;
	boost::unique_lock<boost::mutex> lock(m);

	data->mutex.lock();

	try
	{
		string response(Usuarios_Ativos());

		data->mutex.unlock();

		return response;
	}
	catch (exception &e)
	{
		data->mutex.unlock();

		cerr << "Application Server - get_ativos - " << e.what() << endl;
	}
}


historical_data_reply_t GatewayClient::get_historico(int const id)
{
	boost::mutex m;
	boost::unique_lock<boost::mutex> lock(m);

	data->mutex.lock();

	try
	{
		historical_data_reply_t reply;
		reply.num_samples_available = 0;

		int n = data->LIST_SIZE;

		for (int i = 0; i < n; i++)
		{
			if (data->list[i].header.id == id)
			{
				reply.num_samples_available = 1;
				reply.data[0].header.id = data->list[i].header.id;
				reply.data[0].data.latitude = data->list[i].data.latitude;
				reply.data[0].data.longitude = data->list[i].data.longitude;
				reply.data[0].data.speed = data->list[i].data.speed;
				reply.data[0].data.status = data->list[i].data.status;

				strncpy(reply.data[0].header.imei, data->list[i].header.imei, 16);
				strncpy(reply.data[0].data.timestamp, data->list[i].data.timestamp, 16);

				break;
			}
		}

		data->mutex.unlock();

		return reply;
	}
	catch (exception &e)
	{
		data->mutex.unlock();

		cerr << "Application Server - get_historico - " << e.what() << endl;
	}
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//	string Lista_Ativos;
//
//
//	//Create a shared memory object.
//	shared_memory_object shm
//		(open_only                    // open only
//		, "UsuariosAtivos"              //name
//		, read_write  //read-write mode
//		);
//
//	//Map the whole shared memory in this process
//	mapped_region region
//		(shm                       //What to map
//		, read_write //Map it as read-write
//		);
//
//	//Get the address of the mapped region
//	void * addr = region.get_address();
//
//	//Obtain the shared structure
//	shared_memory_buffer * data = static_cast<shared_memory_buffer*>(addr);
//
//	while (true)
//
//	{
//		data->mutex.lock();
//		Lista_Ativos = Usuarios_Ativos(*data);
//		cout << Lista_Ativos << endl;
//		cout << "REQ_ATIVOS : " << Lista_Ativos << endl;
//
//		data->mutex.unlock();
//	}
//
//	system("PAUSE");
//	return 0;
//}