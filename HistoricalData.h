#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>

static const unsigned MAX_POSITION_SAMPLES = 30;

struct position_t
{
	struct
	{
		int id;
		char imei[16];
	} header;
	struct
	{
		char timestamp[16];
		double latitude;
		double longitude;
		bool status;
		int speed;
	} data;
};

struct historical_data_request_t
{
	int id;
	int num_samples;
};

struct historical_data_reply_t
{
	int num_samples_available;
	position_t data[MAX_POSITION_SAMPLES];
};

struct active_users_t
{
	active_users_t() : num_active_users(0)
	{
		for (unsigned i = 0; i < LIST_SIZE; ++i)
		{
			list[i].header.id = -1;
		}
	}
	int num_active_users;
	enum { LIST_SIZE = 1000000 };
	position_t list[LIST_SIZE];
	boost::interprocess::interprocess_mutex mutex;
};