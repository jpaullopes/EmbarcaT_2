/**
 * @file sensor_temp.c
 * @brief Implementação do driver para o sensor de temperatura
 *
 * Este arquivo implementa as funções para inicialização e leitura
 * do sensor de temperatura interno do Raspberry Pi Pico.
 */

#include "sensor_temp.h"
#include "hardware/adc.h" 

/**
 * @brief Inicializa o ADC para leitura do sensor de temperatura interno.
 *
 * Esta função inicializa o módulo ADC do RP2040 e habilita o sensor 
 * de temperatura interno para permitir as leituras de temperatura.
 */
void sensor_temp_init(void) {
    adc_init();

    adc_set_temp_sensor_enabled(true);
}

/**
 * @brief Lê a temperatura do sensor interno.
 *
 * Esta função realiza a leitura do sensor de temperatura interno do RP2040.
 * O processo envolve:
 * 1. Selecionar o canal ADC 4, que está conectado ao sensor de temperatura
 * 2. Realizar a leitura do ADC
 * 3. Converter o valor digital para tensão
 * 4. Converter a tensão para temperatura em graus Celsius
 *
 * @return A temperatura em graus Celsius.
 */
float sensor_temp_read(void) {
    // Seleciona o canal 4 do ADC onde o sensor de temperatura está conectado
    adc_select_input(4);
    
    uint16_t valor_adc = adc_read();
    
    // Converte o valor do ADC para tensão 
    float voltagem = (valor_adc / 4095.0f) * 3.3f;
    
    // Converte a tensão para temperatura usando a fórmula de calibração do sensor
    float temperatura = 21.0f - (voltagem - 0.706f) / 0.001721f;
    
    return temperatura;
}