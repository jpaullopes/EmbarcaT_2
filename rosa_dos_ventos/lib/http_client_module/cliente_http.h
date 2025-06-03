/**
 * @file cliente_http.h
 * @brief Interface para comunicação HTTP com a nuvem
 * 
 * Este módulo fornece funcionalidades para enviar dados do joystick
 * para um servidor na nuvem através de requisições HTTP.
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
#include "joystick.h"

/**
 * @def PROXY_HOST
/**
 * @brief Endereço do proxy para conexão com o servidor
 */
#define PROXY_HOST "nuvem-jp.zapto.org"

/**
 * @brief Porta do proxy para conexão com o servidor
 */
#define PROXY_PORT 80
                        
/**
 * @brief Envia os dados do joystick para o servidor na nuvem
 * 
 * Esta função envia os dados do joystick para o servidor configurado
 * usando uma requisição HTTP POST.
 * 
 * @param dados_a_enviar Ponteiro para a estrutura com os dados do joystick a serem enviados
 */
void enviar_dados_para_nuvem(const Joystick* dados_a_enviar);

#endif
