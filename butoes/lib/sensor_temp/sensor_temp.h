#ifndef SENSOR_TEMP_H
#define SENSOR_TEMP_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

/**
 * @brief Inicializa o ADC para leitura do sensor de temperatura interno.
 */
void sensor_temp_init(void);

/**
 * @brief Lê a temperatura do sensor interno.
 * @return A temperatura em graus Celsius.
 */
float sensor_temp_read(void);

#endif // SENSOR_TEMP_H