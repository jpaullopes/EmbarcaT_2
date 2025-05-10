#include "sensor_temp.h"
#include "hardware/adc.h" 

void sensor_temp_init(void) {
    adc_init();

    adc_set_temp_sensor_enabled(true);
}

float sensor_temp_read(void) {
    // Seleciona o canal 4 do ADC onde o sensor de temperatura está conectado
    adc_select_input(4);
    
    uint16_t valor_adc = adc_read();
    
    // Converte o valor do ADC para tensão 

    float voltagem = (valor_adc / 4095.0f) * 3.3f;
    
    float temperatura = 21.0f - (voltagem - 0.706f) / 0.001721f;
    
    return temperatura;
}