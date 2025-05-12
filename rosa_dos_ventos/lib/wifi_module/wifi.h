/**
 * @file wifi.h
 * @brief Interface para o módulo WiFi
 * 
 * Este arquivo contém as definições para configuração e conexão
 * do módulo WiFi do dispositivo Raspberry Pi Pico W.
 */
#ifndef WIFI_H
#define WIFI_H

/**
 * @def NOME_REDE_WIFI
 * @brief SSID da rede WiFi para conexão
 */
#define NOME_REDE_WIFI "Tomada preguicosa"

/**
 * @def SENHA_REDE_WIFI
 * @brief Senha da rede WiFi para conexão
 */
#define SENHA_REDE_WIFI "cachorro123"

/**
 * @brief Estabelece uma conexão WiFi com os parâmetros configurados
 * 
 * Esta função inicializa o hardware WiFi e tenta estabelecer 
 * uma conexão com a rede WiFi configurada.
 * 
 * @return 0 se a conexão foi bem-sucedida, qualquer outro valor em caso de falha
 */
int conexao_wifi();

#endif