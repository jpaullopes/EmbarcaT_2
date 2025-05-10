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

// -- DEFINES --
#define PROXY_HOST "crossover.proxy.rlwy.net"      
#define PROXY_PORT 12011
                        


void enviar_dados_para_nuvem(const ButtonStates_t* estados_botoes);

#endif
