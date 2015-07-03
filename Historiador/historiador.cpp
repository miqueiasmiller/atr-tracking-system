/***********************************************************************************/
/*                                Módulo HISTORIADOR                               */
/*                        Trabalho Final - Google Latitude                         */
/*                    Disciplina ATR 2015_1 - Professor André Paim                 */
/*                                                                                 */
/*                         Diogo Figueiredo Silva - 2011021370                     */
/*                       Maxwell Fernandes Martins - 2010018464                    */
/*                                                                                 */
/***********************************************************************************/

/*Declaração das bibliotecas utilizadas:*/
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

/* Inclusao do boost */
#include <boost/interprocess/ipc/message_queue.hpp>

/* headers*/
#include "HistoricalData.h"

using namespace boost::interprocess;
using namespace std;

unsigned int priority, priority_2;
message_queue::size_type recvd_size, recvd_size2; // Fila de mensagens

position_t position; 
vector<std::thread> com_thread; 
mutex m;

historical_data_request_t servidor_historico; 
historical_data_reply_t historico_servidor;

/* Prototipo das funcoes*/
void gateway_communication(position_t position);
void server_communication();
int write_file(position_t t);
int read_file(historical_data_request_t serv_hist_);

/*A função main é responsável por criar as thread que chamam as funções que estabelecem a comunicação entre o historiador e os demais módulos*/
int main()
{
	try
	{
		int thread_count = 0;
		int j=0; 
		com_thread.push_back(thread(gateway_communication, position));
		thread_count++;
		com_thread.push_back(thread(server_communication));
		thread_count++;

		while (j<thread_count) 
		{
			com_thread[j].join();
			j++;
		}
	}
	catch (interprocess_exception &ex)
	{
		message_queue::remove("servapp_historiador");
		message_queue::remove("historiador_servapp");
		message_queue::remove("gateway_historiador");
		cout << ex.what() << endl;
	}
	return 0;
	com_thread.clear();
}

/*A função abaixo estabelece a conexão entre o HISTORIADOR e o GATEWAY através de um Fila de Mensagens*/
void gateway_communication(position_t position) 
{
	int count_gateway = 0; 
	message_queue::remove("gateway_historiador"); //Limpa a fila

	//Criação da Fila de Mensagens na qual o Historiador é o Consumidor e o Gateway é o Produtor.
	message_queue mq_gh(create_only, "gateway_historiador", 100, sizeof(position_t));
	printf ("\nA Fila de Msgs para comunicacao Gateway e Historiador foi criada com sucesso!\n");
	printf ("\nAguardando conexao do Gateway (produtor)...\n");

	while (true)
	{
		//Recebimento das informações vindas do Gateway
		mq_gh.receive(&position, sizeof(position), recvd_size, priority);
		if (count_gateway < 1)
		{
			printf ("\nA conexao com o Gateway foi estabelecida!\n");
			count_gateway++;
		}
		write_file(position);
	}
}

/*A função abaixo estabelece a conexão entre o HISTORIADOR e o SERVIDOR DE APLICAÇÃO atravé de duas filas de mensagens:
uma para requisições e outra para respostas*/
void server_communication() 
{
	try
	{
		int count_server = 0;
		message_queue::remove("servapp_historiador");//Limpa as filas
		message_queue::remove("historiador_servapp");

		//Criação da fila de REQUISIÇÕES: envio de solicitações do servidor de aplicação para o historiador
		message_queue mq_sh(create_only, "servapp_historiador", 100, sizeof(historical_data_request_t));
		printf ("\nA Fila de Requisicoes foi criada com sucesso! serv_app->historiador \n");

		//Criação de fila RESPOSTAS: envio de dados históricos do historiador para o servidor de aplicação
		message_queue mq_hs(create_only, "historiador_servapp", 100, sizeof(historical_data_reply_t));
		printf ("\nA Fila de Respostas foi criada com sucesso! historiador->serv_app \n");
		printf ("\nAguardando conexao do Servidor de Aplicacao... \n");

		while (1)
		{
			//Recebimento das requisições vindas do servidor de aplicação
			mq_sh.receive(&servidor_historico, sizeof(servidor_historico), recvd_size2, priority_2);
			if (count_server < 1)
			{
				printf ("\nA conexao com o Servidor de Aplicacao foi estabelecida! \n \n");
				count_server++;
			}
			read_file(servidor_historico);

			//Envio das respostas contendo dados históricos ao servidor de aplicação
			mq_hs.send(&historico_servidor, sizeof(historico_servidor), 100);
		}
	}

	catch (interprocess_exception &ex)
	{
		cout << ex.what() << endl;
	}
}

//A função abaixo salva as informações de cada cliente recebidas do gateway em um arquivo definido em formato binário*/
int write_file(position_t t) 
{
	unique_lock<mutex> lock(m); //Uso de mutex para fazer com que a escrita dos dados seja uma seção crítica

	stringstream file_name;
	file_name << to_string(t.header.id) << ".dat";

	fstream arquivo(file_name.str(), fstream::out | fstream::in | fstream::binary | fstream::app); 
	if (arquivo.is_open())
	{
		arquivo.write((char*)&t, sizeof(position_t));
		arquivo.close();
	}
	else
	{
		printf ("\nNao foi possivel abrir o arquivo!\n"); 
	}
	return(0);
}

/*A função abaixo lê as informações do arquivo referente ao cliente solicitado*/
int read_file(historical_data_request_t serv_hist_)
{
	unique_lock<mutex> lock(m); //Uso de mutex para fazer com que a leitura dos dados seja uma seção crítica
	stringstream file_name;
	file_name << to_string(serv_hist_.id) << ".dat";

	int counter = 0; //Contador de bytes char; 
	char c; 

	/* Abre o arquivo para descobrir seu tamanho */
	fstream arquivo(file_name.str(), fstream::out | fstream::in | fstream::binary | fstream::app);
	if (arquivo.is_open())
	{
		while (arquivo.get(c)) //Descobre o tamanho do arquivo
		{
			counter++;
		}
	}

	int num_rec = counter / sizeof(position_t); //Descobre a quantidade de linhas no arquivo lido 
	arquivo.close();

	/* Abre o arquivo para leitura */
	fstream arquivo2(file_name.str(), fstream::out | fstream::in | fstream::binary | fstream::app); 
	if (arquivo2.is_open())
	{
		int num_samples = serv_hist_.num_samples;
		
		if (num_samples > 30)
			num_samples = 30;

		/*As condições abaixão verificam a quantidade de dados históricos disponíveis. Vale lembrar que o número máx de posições retornadas é 30*/
		if (num_rec >= num_samples) 
		{
			arquivo2.seekp((num_rec - num_samples) * sizeof(position_t));
			num_rec = num_samples;
		}

		/*Número de dados que serão retornados*/
		historico_servidor.num_samples_available = num_rec;

		/*O loop abaixo carrega a quantidade de dados requisitados em uma variável para ser enviada ao servidor da aplicação*/
		for (int i = 0; i < num_rec; ++i)
		{
			arquivo2.read((char*)&historico_servidor.data[i], sizeof(position_t));
		}

		cout << "\n\nOs dados historicos do celular com ID " << historico_servidor.data[0].header.id << " e IMEI " << historico_servidor.data[0].header.imei << " foram enviados para o servidor de aplicacao!" << endl;
		cout << "\nO numero de posicoes historicas enviadas foi " << num_rec << endl << endl;
		arquivo2.close();
	}
	else
	{
		printf("\nOcorreu um erro na abertura do arquivo para leitura!\n");
	}
	return(0);
}