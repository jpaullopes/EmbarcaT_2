#include "buttons.h"
#include "hardware/gpio.h"

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