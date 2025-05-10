#ifndef JOYSTICK_H
#define JOYSTICK_H

#define PINO_JOY_X 26 // Pino de leitura do eixo X do joystick (conectado ao ADC)
#define PINO_JOY_Y 27 // Pino de leitura do eixo Y do joystick (conectado ao ADC)
#define ADC_CHANNEL_X 0 // Canal ADC para o eixo X do joystick
#define ADC_CHANNEL_Y 1 // Canal ADC para o eixo Y do joystick
#define PINO_BUTTON 22 // Pino de leitura do botão do joystick 

/**
 * @brief Estrutura para representar um joystick
 */
typedef struct {
    int x_position;        
    int y_position;
    uint8_t button_pressed; // 1 se o botão estiver pressionado, 0 caso contrário      
} Joystick;

/**
 * @brief Inicializa o joystick
 */
void joystick_init(void);

/**
 * @brief Lê os valores do joystick
 * @param joystick Ponteiro para a estrutura do joystick
 */
void read_joystick(Joystick *joystick);


#endif // JOYSTICK_H