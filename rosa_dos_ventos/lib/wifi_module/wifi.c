/**
 * @file wifi.c
 * @brief Implementação das funções do módulo WiFi
 * 
 * Este arquivo contém a implementação das funções para inicialização
 * e conexão do módulo WiFi do Raspberry Pi Pico W.
 */
#include "wifi.h"
#include "pico/cyw43_arch.h"

/**
 * @brief Inicializa a conexão Wi-Fi
 * 
 * Esta função realiza as seguintes operações:
 * 1. Inicializa o hardware WiFi CYW43
 * 2. Ativa o modo estação (cliente)
 * 3. Tenta conectar à rede WiFi configurada com timeout de 10 segundos
 * 
 * @return 0 se a conexão for bem-sucedida, -1 caso contrário
 */
int conexao_wifi() {

    int conexao;

    while (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Conectando ao Wi-Fi '%s'...\n", NOME_REDE_WIFI);
    
    conexao = cyw43_arch_wifi_connect_timeout_ms(NOME_REDE_WIFI, SENHA_REDE_WIFI, CYW43_AUTH_WPA2_AES_PSK, 10000);

    char *menssagem = (conexao == 0) ? "Wifi Conectado...\n" : "Falha ao Conectar...\n";
    printf(menssagem);
    return conexao;
}