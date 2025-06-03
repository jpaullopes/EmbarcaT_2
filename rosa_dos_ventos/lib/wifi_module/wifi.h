/**
 * @file wifi.h
 * @brief Interface do módulo Wi-Fi
 *
 * Este arquivo define as constantes e funções para inicialização
 * e controle do módulo Wi-Fi do Raspberry Pi Pico W.
 */

#ifndef WIFI_H
#define WIFI_H

/**
 * @defgroup WIFI_MODULE Módulo Wi-Fi
 * @{
 */

/**
 * @brief Nome da rede Wi-Fi para conexão
 */
#define NOME_REDE_WIFI "teste"

/**
 * @brief Senha da rede Wi-Fi para conexão
 */
#define SENHA_REDE_WIFI "teste123"

/**
 * @brief Inicializa e conecta o módulo Wi-Fi à rede configurada
 *
 * Esta função inicializa o módulo Wi-Fi do Raspberry Pi Pico W,
 * configura o modo estação e tenta conectar à rede Wi-Fi especificada
 * pelos defines NOME_REDE_WIFI e SENHA_REDE_WIFI.
 *
 * @return 0 se a conexão for bem-sucedida, -1 caso contrário
 */
int conexao_wifi();

/** @} */ // Fim do grupo WIFI_MODULE

#endif