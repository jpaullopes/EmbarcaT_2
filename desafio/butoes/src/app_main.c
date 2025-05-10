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

// Configurações de comunicação
#define INTERVALO_ENVIO_DADOS_BOTOES_MS 1000

// Definições para Tasks do FreeRTOS
#define BUTTON_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)
#define WIFI_TASK_PRIORITY     (tskIDLE_PRIORITY + 2)
#define BUTTON_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define WIFI_TASK_STACK_SIZE   configMINIMAL_STACK_SIZE * 2

// Fila para comunicação entre tasks
static QueueHandle_t xButtonEventQueue = NULL;

// Variáveis Globais para Wi-Fi
static bool wifi_conectado_status_botoes = false;
static uint32_t ultimo_envio_botoes_ms = 0;

// Protótipos das Tasks
static void button_task(void *pvParameters);
static void wifi_task(void *pvParameters);

// Protótipos de Funções Locais
static bool tentar_conectar_wifi_botoes_freertos(void);

int main(void) {
    stdio_init_all();
    printf("Sistema de Botões inicializando com FreeRTOS...\n");

    buttons_init();
    printf("Botões GPIO inicializados.\n");

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

    while (true) {
        buttons_read(&estado_atual_botoes);

        bool mudou = (estado_atual_botoes.button_a_pressed != estado_anterior_botoes.button_a_pressed ||
                      estado_atual_botoes.button_b_pressed != estado_anterior_botoes.button_b_pressed);

        if (mudou) {
            printf("Mudança Botões (Core %d): A=%s, B=%s\n", get_core_num(),
                   estado_atual_botoes.button_a_pressed ? "ON" : "OFF",
                   estado_atual_botoes.button_b_pressed ? "ON" : "OFF");

            // Envia o novo estado para a fila
            if (xQueueSend(xButtonEventQueue, &estado_atual_botoes, (TickType_t)10) != pdPASS) {
                printf("Falha ao enviar para a fila de botões!\n");
            }
            estado_anterior_botoes = estado_atual_botoes;
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
                    printf("Enviando dados dos botões para a nuvem (Core %d)...\n", get_core_num());

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