#pragma once

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
	int id; // identificador do dispositivo
	int num_samples; // quantidade de amostras solicitadas
};

struct historical_data_reply_t
{
	int num_samples_available; // número de amostras disponibilizadas
	position_t data[MAX_POSITION_SAMPLES]; // vetor de amostras
};