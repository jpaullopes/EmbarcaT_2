/**
 * @file app_main.c
 * @brief Aplicação principal do sistema de monitoramento de botões e temperatura
 * @author João Paulo Lopes
 * @date Maio 2025
 *
 * Este arquivo implementa a aplicação principal que monitora o estado dos botões
 * e a temperatura, enviando os dados para a nuvem através de uma conexão Wi-Fi.
 * A aplicação utiliza o sistema operacional FreeRTOS para gerenciar múltiplas tarefas.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"

#include "lwip/netif.h"
#include "lwip/ip_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "buttons.h"
#include "cliente_http.h"
#include "wifi.h"
#include "sensor_temp.h"

/**
 * @defgroup APP_MAIN Aplicação Principal
 * @{
 */

/**
 * @brief Intervalo em milissegundos para envio de dados para a nuvem
 */
#define INTERVALO_ENVIO_DADOS_BOTOES_MS 1000

/**
 * @brief Prioridades e tamanhos de stack para tasks do FreeRTOS
 * @{
 */
#define BUTTON_TASK_PRIORITY   (tskIDLE_PRIORITY + 1) /**< Prioridade da task de botões */
#define WIFI_TASK_PRIORITY     (tskIDLE_PRIORITY + 2) /**< Prioridade da task de Wi-Fi (maior para garantir envio de dados) */
#define BUTTON_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 256) /**< Tamanho da stack da task de botões */
#define WIFI_TASK_STACK_SIZE   configMINIMAL_STACK_SIZE * 2     /**< Tamanho da stack da task de Wi-Fi */
/** @} */

/**
 * @brief Fila para comunicação entre tasks
 * 
 * Esta fila é usada para passar eventos de mudança dos botões da task de botões
 * para a task de Wi-Fi, que então envia os dados para a nuvem.
 */
static QueueHandle_t xButtonEventQueue = NULL;

/**
 * @brief Variáveis globais para gerenciamento do estado Wi-Fi
 * @{
 */
static bool wifi_conectado_status_botoes = false;   /**< Indica se o Wi-Fi está conectado */
static uint32_t ultimo_envio_botoes_ms = 0;         /**< Timestamp do último envio de dados */
/** @} */

/**
 * @brief Protótipos das tasks FreeRTOS
 * @{
 */
/**
 * @brief Task responsável por monitorar os botões e enviar atualizações para a task Wi-Fi
 * @param pvParameters Parâmetros passados para a task (não utilizado)
 */
static void button_task(void *pvParameters);

/**
 * @brief Task responsável por gerenciar a conexão Wi-Fi e enviar dados para a nuvem
 * @param pvParameters Parâmetros passados para a task (não utilizado)
 */
static void wifi_task(void *pvParameters);
/** @} */

/**
 * @brief Protótipos de funções auxiliares
 * @{
 */
/**
 * @brief Tenta estabelecer conexão Wi-Fi
 * @return true se a conexão foi bem-sucedida, false caso contrário
 */
static bool tentar_conectar_wifi_botoes_freertos(void);

int main(void) {
    stdio_init_all();
    printf("Sistema de Botões e Temperatura inicializando com FreeRTOS...\n");

    buttons_init();
    printf("Botões GPIO inicializados.\n");
    sensor_temp_init();
    printf("Sensor de temperatura inicializado.\n");


    // Cria a fila para eventos dos botões
    xButtonEventQueue = xQueueCreate(5, sizeof(ButtonStates_t));
    if (xButtonEventQueue == NULL) {
        printf("Falha ao criar a fila de eventos dos botões!\n");
        while (1);
    }

    // Cria a task de leitura dos botões
    xTaskCreate(button_task, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL);

    TaskHandle_t wifiTaskHandle;
    xTaskCreate(wifi_task, "WifiTask", WIFI_TASK_STACK_SIZE, NULL, WIFI_TASK_PRIORITY, &wifiTaskHandle);

    printf("Scheduler FreeRTOS iniciando...\n");
    vTaskStartScheduler();

    while (true);
    return 0;
}

// Implementação da Task de Leitura dos Botões
static void button_task(void *pvParameters) {
    printf("Button Task iniciada no Core %d\n", get_core_num());
    ButtonStates_t estado_atual_botoes;
    ButtonStates_t estado_anterior_botoes;

    estado_anterior_botoes.button_a_pressed = false;
    estado_anterior_botoes.button_b_pressed = true;

    buttons_read(&estado_anterior_botoes);

    while (true) {
        buttons_read(&estado_atual_botoes);
        estado_atual_botoes.temperature = sensor_temp_read();

        bool mudou = (estado_atual_botoes.button_a_pressed != estado_anterior_botoes.button_a_pressed ||
                      estado_atual_botoes.button_b_pressed != estado_anterior_botoes.button_b_pressed);

        if (mudou) {
            printf("Mudança Botões (Core %d): A=%s, B=%s. Temp: %.2f C\n", get_core_num(), // <<< ATUALIZADO
                   estado_atual_botoes.button_a_pressed ? "ON" : "OFF",
                   estado_atual_botoes.button_b_pressed ? "ON" : "OFF",
                   estado_atual_botoes.temperature);

            if (xQueueSend(xButtonEventQueue, &estado_atual_botoes, (TickType_t)10) != pdPASS) {
                printf("Falha ao enviar para a fila de botões!\n");
            }
            estado_anterior_botoes.button_a_pressed = estado_atual_botoes.button_a_pressed;
            estado_anterior_botoes.button_b_pressed = estado_atual_botoes.button_b_pressed;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Implementação da Task de Wi-Fi e Envio de Dados
static bool tentar_conectar_wifi_botoes_freertos(void) {
    if (conexao_wifi() == 0) {
        printf("Botões (Core %d): WiFi conectado com sucesso!\n", get_core_num());
        printf("Botões (Core %d): IP do dispositivo: %s\n", get_core_num(), ipaddr_ntoa(netif_ip4_addr(netif_default)));
        return true;
    } else {
        printf("Botões (Core %d): Falha ao conectar ao WiFi inicialmente.\n", get_core_num());
        return false;
    }
}

static void wifi_task(void *pvParameters) {
    printf("WiFi Task iniciada no Core %d\n", get_core_num());
    ButtonStates_t estado_recebido_botoes;

    wifi_conectado_status_botoes = tentar_conectar_wifi_botoes_freertos();

    while (true) {
        cyw43_arch_poll();

        // Tenta receber da fila
        if (xQueueReceive(xButtonEventQueue, &estado_recebido_botoes, pdMS_TO_TICKS(100))) {
            // Se recebeu algo da fila e o Wi-Fi está conectado
            if (wifi_conectado_status_botoes) {
                uint32_t tempo_atual_ms = to_ms_since_boot(get_absolute_time());
                if (tempo_atual_ms - ultimo_envio_botoes_ms >= INTERVALO_ENVIO_DADOS_BOTOES_MS) {
                    printf("Enviando dados (botões e temp: %.2fC) para a nuvem (Core %d)...\n",estado_recebido_botoes.temperature, get_core_num());
                    enviar_dados_para_nuvem(&estado_recebido_botoes);

                    ultimo_envio_botoes_ms = tempo_atual_ms;
                }
            }
        }

        // Lógica de reconexão ou status do Wi-Fi
        if (!wifi_conectado_status_botoes) {
            static uint32_t ultimo_log_wifi_falhou_botoes = 0;
            if (to_ms_since_boot(get_absolute_time()) - ultimo_log_wifi_falhou_botoes > 10000) {
                printf("Botões (Core %d): WiFi não conectado. Tentando reconectar...\n", get_core_num());
                wifi_conectado_status_botoes = tentar_conectar_wifi_botoes_freertos();
                ultimo_log_wifi_falhou_botoes = to_ms_since_boot(get_absolute_time());
            }
        }
    }
}