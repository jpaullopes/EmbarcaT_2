/**
 * @file joystick.h
 * @brief Interface para controle e leitura do joystick
 * 
 * Este arquivo contém as definições para interagir com o joystick,
 * incluindo inicialização e leitura das posições X/Y e estado do botão.
 */
#ifndef JOYSTICK_H
#define JOYSTICK_H

/**
 * @def PINO_JOY_X
 * @brief Pino GPIO utilizado para ler o eixo X do joystick
 */
#define PINO_JOY_X 26

/**
 * @def PINO_JOY_Y
 * @brief Pino GPIO utilizado para ler o eixo Y do joystick
 */
#define PINO_JOY_Y 27

/**
 * @def ADC_CHANNEL_X
 * @brief Canal ADC utilizado para ler o eixo X do joystick
 */
#define ADC_CHANNEL_X 0

/**
 * @def ADC_CHANNEL_Y
 * @brief Canal ADC utilizado para ler o eixo Y do joystick
 */
#define ADC_CHANNEL_Y 1

/**
 * @def PINO_BUTTON
 * @brief Pino GPIO utilizado para ler o botão do joystick
 */
#define PINO_BUTTON 22 

/**
 * @brief Estrutura para representar um joystick
 */
typedef struct {
    int x_position;         /**< Posição no eixo X (0-100) */
    int y_position;         /**< Posição no eixo Y (0-100) */
    uint8_t button_pressed; /**< 1 se o botão estiver pressionado, 0 caso contrário */
} Joystick;

/**
 * @brief Inicializa o joystick
 * 
 * Configura os pinos GPIO e inicializa o ADC para leitura do joystick.
 * Deve ser chamada antes de qualquer outra função do joystick.
 */
void joystick_init(void);

/**
 * @brief Lê os valores do joystick
 * 
 * Captura os valores atuais de posição X, Y e o estado do botão.
 * Os valores X e Y são normalizados para o intervalo de 0-100.
 * 
 * @param joystick Ponteiro para a estrutura onde serão armazenados os valores lidos
 */
void read_joystick(Joystick *joystick);


#endif // JOYSTICK_H