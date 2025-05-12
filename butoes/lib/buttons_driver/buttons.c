/**
 * @file buttons.c
 * @brief Implementação do driver para botões
 *
 * Este arquivo implementa as funções para inicialização e leitura
 * dos botões conectados ao microcontrolador Raspberry Pi Pico.
 */

#include "buttons.h"
#include "hardware/gpio.h"

/**
 * @brief Inicializa os pinos GPIO para os botões.
 *
 * Esta função configura os pinos GPIO para os botões A e B como entradas
 * com resistores pull-up internos habilitados. Isso permite detectar quando
 * os botões são pressionados (conectados ao GND).
 */
void buttons_init(void) {
    // Botão A
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN); // Assume que o botão conecta o pino ao GND quando pressionado

    // Botão B
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN); // Assume que o botão conecta o pino ao GND quando pressionado
    
    // Para debouncing, você poderia inicializar timers ou interrupções aqui,
    // mas para manter simples, faremos debouncing por software na task se necessário,
    // ou apenas leituras periódicas.
}

/**
 * @brief Lê o estado atual dos botões.
 *
 * Esta função lê o estado atual dos pinos GPIO conectados aos botões A e B
 * e armazena os resultados na estrutura ButtonStates_t fornecida.
 * Com resistores pull-up, quando o botão é pressionado, o pino vai para estado LOW.
 *
 * @param states Ponteiro para a estrutura ButtonStates_t onde o estado será armazenado
 * @note O estado de um botão será considerado como pressionado (true) quando o pino estiver em LOW
 */
void buttons_read(ButtonStates_t *states) {
    if (states == NULL) {
        return;
    }
    // Com pull-up, o pino vai para LOW (0) quando o botão é pressionado.
    // gpio_get() retorna true (1) se HIGH, false (0) se LOW.
    // Portanto, invertemos a lógica para 'pressed' ser true quando o pino está em LOW.
    states->button_a_pressed = !gpio_get(BUTTON_A_PIN);
    states->button_b_pressed = !gpio_get(BUTTON_B_PIN);
}