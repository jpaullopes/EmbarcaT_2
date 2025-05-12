/**
 * @file buttons.h
 * @brief Interface do driver para botões
 *
 * Este arquivo define as constantes e funções para controle
 * dos botões conectados ao microcontrolador Raspberry Pi Pico.
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include "pico/stdlib.h" // Para bool e tipos uint

/**
 * @defgroup BUTTONS_DRIVER Driver de Botões
 * @{
 */

/**
 * @brief Número do pino GPIO conectado ao Botão A
 */
#define BUTTON_A_PIN 5

/**
 * @brief Número do pino GPIO conectado ao Botão B
 */
#define BUTTON_B_PIN 6

/**
 * @brief Estrutura para armazenar o estado dos botões e temperatura
 *
 * Esta estrutura mantém o estado atual dos botões A e B (pressionado ou não)
 * e o valor atual de temperatura lido pelo sensor.
 */
typedef struct {
    bool button_a_pressed;    /**< Estado do botão A: true se pressionado, false caso contrário */
    bool button_b_pressed;    /**< Estado do botão B: true se pressionado, false caso contrário */
    float temperature;        /**< Temperatura atual em graus Celsius */
} ButtonStates_t;

/**
 * @brief Inicializa os pinos GPIO para os botões.
 * Configura os pinos como entrada com pull-up interno.
 */
void buttons_init(void);

/**
 * @brief Lê o estado atual dos botões.
 * @param states Ponteiro para a estrutura ButtonStates_t onde o estado será armazenado.
 */
void buttons_read(ButtonStates_t *states);

/** @} */ // Fim do grupo BUTTONS_DRIVER

#endif // BUTTONS_H