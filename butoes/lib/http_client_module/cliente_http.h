/**
 * @file cliente_http.h
 * @brief Módulo de cliente HTTP para envio de dados para a nuvem
 *
 * Este módulo implementa um cliente HTTP simples utilizando lwIP para
 * enviar dados dos botões e temperatura para um servidor remoto via HTTP.
 */

#ifndef CLIENTE_HTTP_H
#define CLIENTE_HTTP_H

// -- INCLUDES --
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
#include "buttons.h"

/**
 * @defgroup HTTP_CLIENT Módulo Cliente HTTP
 * @{
 */

/**
 * @brief Endereço do proxy para conexão com o servidor
 */
#define PROXY_HOST "crossover.proxy.rlwy.net"

/**
 * @brief Porta do proxy para conexão com o servidor
 */
#define PROXY_PORT 12011
                        

/**
 * @brief Envia os dados dos botões e temperatura para o servidor na nuvem
 *
 * @param estados_botoes Ponteiro para a estrutura com os estados dos botões e temperatura
 *
 * @note Esta função inicia o processo de envio de dados para o servidor, 
 *       realizando primeiro a resolução DNS e depois a conexão TCP.
 */
void enviar_dados_para_nuvem(const ButtonStates_t* estados_botoes);

/** @} */ // Fim do grupo HTTP_CLIENT

#endif
