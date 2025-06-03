/**
 * @file app_main.c
 * @brief Aplicação principal do projeto Rosa dos Ventos
 *
 * Este arquivo contém a implementação principal do sistema Rosa dos Ventos,
 * que monitora um joystick, determina sua direção e envia os dados para a nuvem.
 *
 * @author João Paulo Lopes
 * @date Maio 2025
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "joystick.h"
#include "cliente_http.h"
#include "wifi.h"

/**
 * @def DEAD_ZONE_MIN
 * @brief Limite inferior da zona morta do joystick (0-100)
 */
#define DEAD_ZONE_MIN 35

/**
 * @def DEAD_ZONE_MAX
 * @brief Limite superior da zona morta do joystick (0-100)
 */
#define DEAD_ZONE_MAX 65

/**
 * @def INTERVALO_ENVIO_DADOS_MS
 * @brief Intervalo mínimo entre envios sucessivos de dados para a nuvem (ms)
 */
#define INTERVALO_ENVIO_DADOS_MS 1000

/**
 * @brief Direções possíveis do joystick.
 */
typedef enum {
    CENTRO,     /**< Posição central/neutra do joystick */
    LESTE,      /**< Joystick movido para a direita */
    OESTE,      /**< Joystick movido para a esquerda */
    NORTE,      /**< Joystick movido para cima */
    SUL,        /**< Joystick movido para baixo */
    NORDESTE,   /**< Joystick movido na diagonal superior direita */
    NOROESTE,   /**< Joystick movido na diagonal superior esquerda */
    SUDESTE,    /**< Joystick movido na diagonal inferior direita */
    SUDOESTE,   /**< Joystick movido na diagonal inferior esquerda */
    DIRECAO_DESCONHECIDA /**< Estado não reconhecido do joystick */
} JoystickDirection;

/**
 * @brief Estado do joystick em um determinado momento.
 */
typedef struct {
    JoystickDirection direcao;   /**< Direção calculada do joystick */
    int x_position;              /**< Posição no eixo X (0-100) */
    int y_position;              /**< Posição no eixo Y (0-100) */
    uint8_t button_pressed;      /**< Estado do botão (0=solto, 1=pressionado) */
} EstadoJoystick;

/** @brief Estado atual do joystick lido pelos sensores */
static EstadoJoystick estado_atual_joystick;

/** @brief Estado anterior do joystick para detecção de mudanças */
static EstadoJoystick estado_anterior_joystick;

/** @brief Status da conexão WiFi */
static bool wifi_conectado_status = false;

/** @brief Timestamp do último envio de dados para a nuvem */
static uint32_t ultimo_envio_dados_ms = 0;

/**
 * @brief Inicializa todos os componentes do sistema
 */
static void inicializar_sistema(void);

/**
 * @brief Tenta estabelecer uma conexão WiFi inicial
 * @return true se a conexão foi estabelecida com sucesso, false caso contrário
 */
static bool tentar_conectar_wifi_inicialmente(void);

/**
 * @brief Calcula a direção do joystick com base nas coordenadas X e Y
 * @param x Posição X normalizada (0-100)
 * @param y Posição Y normalizada (0-100)
 * @return A direção calculada do joystick
 */
static JoystickDirection calcular_direcao_joystick(int x, int y);

/**
 * @brief Converte uma direção do joystick para sua representação em texto
 * @param dir Direção do joystick a ser convertida
 * @return String representando a direção
 */
static const char* converter_direcao_para_string(JoystickDirection dir);

/**
 * @brief Verifica se houve mudança no estado do joystick desde a última leitura
 * @return true se houve mudança, false caso contrário
 */
static bool houve_mudanca_estado_joystick(void);

/**
 * @brief Lê os dados do joystick e atualiza o estado atual
 */
static void ler_e_processar_joystick(void);

/**
 * @brief Tenta enviar dados do joystick para a nuvem se houver mudança e o intervalo permitir
 */
static void tentar_enviar_dados_joystick(void);

/**
 * @brief Função principal do programa.
 */
int main(void) {
    inicializar_sistema();
    memset(&estado_anterior_joystick, 0, sizeof(EstadoJoystick));
    estado_anterior_joystick.direcao = DIRECAO_DESCONHECIDA;
    estado_anterior_joystick.button_pressed = 2;

    printf("Iniciando loop principal...\n");
    while (true) {
        cyw43_arch_poll();
        ler_e_processar_joystick();
        if (wifi_conectado_status) {
            tentar_enviar_dados_joystick();
        } else {
            static uint32_t ultimo_log_wifi_falhou = 0;
            if (to_ms_since_boot(get_absolute_time()) - ultimo_log_wifi_falhou > 10000) {
                printf("WiFi não conectado. Não tentando enviar dados.\n");
                ultimo_log_wifi_falhou = to_ms_since_boot(get_absolute_time());
            }
        }
        sleep_ms(50);
    }
    return 0;
}

static void inicializar_sistema(void) {
    stdio_init_all();
    sleep_ms(1000);
    joystick_init();
    printf("Joystick inicializado.\n");
    
    // Inicializar conexão WiFi
    wifi_conectado_status = tentar_conectar_wifi_inicialmente();
}

static bool tentar_conectar_wifi_inicialmente(void) {
    if (conexao_wifi() == 0) {
        printf("WiFi conectado com sucesso!\n");
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(netif_ip4_addr(netif_default)));
        return true;
    } else {
        printf("Falha ao conectar ao WiFi inicialmente.\n");
        return false;
    }
}

/**
 * @brief Calcula a direção do joystick com base nos valores de X e Y.
 */
static JoystickDirection calcular_direcao_joystick(int x, int y) {
    bool x_dead = (x >= DEAD_ZONE_MIN && x <= DEAD_ZONE_MAX);
    bool y_dead = (y >= DEAD_ZONE_MIN && y <= DEAD_ZONE_MAX);
    bool x_pos = (x > DEAD_ZONE_MAX); bool x_neg = (x < DEAD_ZONE_MIN);
    bool y_pos = (y > DEAD_ZONE_MAX); bool y_neg = (y < DEAD_ZONE_MIN);

    if (x_dead && y_dead) return CENTRO;
    if (y_pos) { if (x_neg) return NOROESTE; if (x_pos) return NORDESTE; return NORTE; }
    if (y_neg) { if (x_neg) return SUDOESTE; if (x_pos) return SUDESTE; return SUL; }
    if (x_pos) return LESTE; if (x_neg) return OESTE;
    return DIRECAO_DESCONHECIDA;
}

static const char* converter_direcao_para_string(JoystickDirection dir) {
    switch (dir) {
        case CENTRO: return "Centro"; case LESTE: return "Leste"; case OESTE: return "Oeste";
        case NORTE: return "Norte"; case SUL: return "Sul"; case NORDESTE: return "Nordeste";
        case NOROESTE: return "Noroeste"; case SUDESTE: return "Sudeste"; case SUDOESTE: return "Sudoeste";
        default: return "Desconhecido";
    }
}

/**
 * @brief Verifica se houve mudança no estado do joystick.
 */
static bool houve_mudanca_estado_joystick(void) {
    return (estado_atual_joystick.direcao != estado_anterior_joystick.direcao ||
            estado_atual_joystick.button_pressed != estado_anterior_joystick.button_pressed ||
            estado_atual_joystick.x_position != estado_anterior_joystick.x_position ||
            estado_atual_joystick.y_position != estado_anterior_joystick.y_position);
}

/**
 * @brief Lê e processa o estado atual do joystick.
 */
static void ler_e_processar_joystick(void) {
    Joystick dados_brutos_joystick;
    read_joystick(&dados_brutos_joystick);
    estado_atual_joystick.x_position = dados_brutos_joystick.x_position;
    estado_atual_joystick.y_position = dados_brutos_joystick.y_position;
    estado_atual_joystick.button_pressed = dados_brutos_joystick.button_pressed;
    estado_atual_joystick.direcao = calcular_direcao_joystick(
        estado_atual_joystick.x_position, estado_atual_joystick.y_position);
}

/**
 * @brief Tenta enviar os dados do joystick para a nuvem se houver mudança.
 */
static void tentar_enviar_dados_joystick(void) {
    uint32_t tempo_atual_ms = to_ms_since_boot(get_absolute_time());

    if (houve_mudanca_estado_joystick()) {
        printf("Mudança Joystick: X=%d, Y=%d, Btn=%d, Dir=%s\n",
               estado_atual_joystick.x_position, estado_atual_joystick.y_position,
               estado_atual_joystick.button_pressed,
               converter_direcao_para_string(estado_atual_joystick.direcao));

        if (tempo_atual_ms - ultimo_envio_dados_ms >= INTERVALO_ENVIO_DADOS_MS) {
            printf("Enviando dados para a nuvem...\n");
            Joystick dados_para_envio;
            dados_para_envio.x_position = estado_atual_joystick.x_position;
            dados_para_envio.y_position = estado_atual_joystick.y_position;
            dados_para_envio.button_pressed = estado_atual_joystick.button_pressed;
            enviar_dados_para_nuvem(&dados_para_envio);
            ultimo_envio_dados_ms = tempo_atual_ms;
        }
        estado_anterior_joystick = estado_atual_joystick;
    }
}