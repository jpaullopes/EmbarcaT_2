/**
 * @file sensor_temp.h
 * @brief Interface do driver para o sensor de temperatura
 *
 * Este arquivo define as funções para inicialização e leitura
 * do sensor de temperatura interno do Raspberry Pi Pico.
 */

#ifndef SENSOR_TEMP_H
#define SENSOR_TEMP_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

/**
 * @defgroup TEMPERATURE_SENSOR Driver de Sensor de Temperatura
 * @{
 */

/**
 * @brief Inicializa o ADC para leitura do sensor de temperatura interno.
 *
 * Esta função configura o conversor analógico-digital (ADC) e habilita
 * o sensor de temperatura interno do RP2040 para medições.
 */
void sensor_temp_init(void);

/**
 * @brief Lê a temperatura do sensor interno.
 *
 * Esta função seleciona o canal ADC apropriado, realiza a leitura do 
 * sensor de temperatura interno e converte o valor lido para temperatura em graus Celsius.
 *
 * @return A temperatura em graus Celsius.
 */
float sensor_temp_read(void);

/** @} */ // Fim do grupo TEMPERATURE_SENSOR

#endif // SENSOR_TEMP_H