/***********************************************************************************/
/*                          Módulo SERVIDOR DE APLICAÇÃO                           */
/*                        Trabalho Final - Google Latitude                         */
/*                    Disciplina ATR 2015_1 - Professor André Paim                 */
/*                                                                                 */
/*                      Miquéias Miller Penha Dias - 2014129449                    */
/*                       Daniel                    */
/*                                                                                 */
/***********************************************************************************/

#pragma once

#include "ProxyServer.h"

int main(int argc, char *argv[])
{
	int port = 9002;

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	ProxyServer proxy(port);
	proxy.start();
}