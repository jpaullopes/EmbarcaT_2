#ifndef BUTTONS_H
#define BUTTONS_H

#include "pico/stdlib.h" // Para bool e tipos uint

// Definição dos pinos para os botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Estrutura para armazenar o estado dos botões
typedef struct {
    bool button_a_pressed;
    bool button_b_pressed;
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

#endif // BUTTONS_H