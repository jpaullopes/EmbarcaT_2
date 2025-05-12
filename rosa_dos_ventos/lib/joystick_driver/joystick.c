/**
 * @file joystick.c
 * @brief Implementação das funções do driver do joystick
 */
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "joystick.h" 
#include <stdio.h>   
#include <stdlib.h>

/**
 * @brief Inicializa o joystick
 * 
 * Configura o ADC e os pinos GPIO necessários para o joystick:
 * - Inicializa o ADC
 * - Configura os pinos analógicos X e Y
 * - Configura o pino do botão com pull-up interno
 */
void joystick_init(void){
    adc_init();                   // Inicializa o conversor analógico-digital
    adc_gpio_init(PINO_JOY_X);    // Configura pino X como entrada analógica
    adc_gpio_init(PINO_JOY_Y);    // Configura pino Y como entrada analógica
    gpio_init(PINO_BUTTON);       // Inicializa o pino do botão
    gpio_set_dir(PINO_BUTTON, GPIO_IN); // Define como entrada
    gpio_pull_up(PINO_BUTTON);    // Habilita o resistor de pull-up interno
}

/**
 * @brief Lê os valores do joystick
 * 
 * Realiza a leitura dos valores de X e Y do joystick através do ADC 
 * e do estado do botão através da GPIO. Os valores são normalizados 
 * para uma escala de 0-100 para facilitar o processamento.
 * 
 * @param joystick Ponteiro para a estrutura onde serão armazenados os valores lidos
 */
void read_joystick(Joystick *joystick){
    // Lê os valores analógicos do eixo X
    adc_select_input(ADC_CHANNEL_X); 
    uint16_t x_value = adc_read();

    // Lê os valores analógicos do eixo Y
    adc_select_input(ADC_CHANNEL_Y); 
    uint16_t y_value = adc_read();

    // Normaliza os valores para a faixa de 0-100
    joystick->x_position = (x_value * 100) / 4095;
    joystick->y_position = (y_value * 100) / 4095;

    // Lê o estado do botão (ativo em nível baixo devido ao pull-up)
    if(gpio_get(PINO_BUTTON) == 0){ 
        joystick->button_pressed = 1; // Botão pressionado
    } else {
        joystick->button_pressed = 0; // Botão não pressionado
    }
}