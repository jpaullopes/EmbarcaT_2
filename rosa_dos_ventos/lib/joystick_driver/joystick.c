#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "joystick.h" 
#include <stdio.h>   
#include <stdlib.h>

/**
 * @brief Inicializa o joystick
 */
void joystick_init(void){
    adc_init(); 
    adc_gpio_init(PINO_JOY_X); 
    adc_gpio_init(PINO_JOY_Y); 
    gpio_init(PINO_BUTTON); 
    gpio_set_dir(PINO_BUTTON, GPIO_IN); 
    gpio_pull_up(PINO_BUTTON); 
}

/**
 * @brief Lê os valores do joystick
 * @param joystick Ponteiro para a estrutura do joystick
 */
void read_joystick(Joystick *joystick){
    // Lê os valores do joystick
    adc_select_input(ADC_CHANNEL_X); 
    uint16_t x_value = adc_read();

    adc_select_input(ADC_CHANNEL_Y); 
    uint16_t y_value = adc_read();

    joystick->x_position = (x_value * 100) / 4095;
    joystick->y_position = (y_value * 100) / 4095;

    // Lê o estado do botão
    if(gpio_get(PINO_BUTTON) == 0){ 
        joystick->button_pressed = 1; // Botão pressionado
    } else {
        joystick->button_pressed = 0; // Botão não pressionado
    }
}