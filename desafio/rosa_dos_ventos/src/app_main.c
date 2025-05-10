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

// --- Configurações do Joystick ---
// Zona morta define um intervalo onde pequenas variações são ignoradas
#define DEAD_ZONE_MIN 35    // Limite inferior da zona morta (0-100)
#define DEAD_ZONE_MAX 65    // Limite superior da zona morta (0-100)

// --- Configurações de Comunicação ---
#define INTERVALO_ENVIO_DADOS_MS 1000  // Intervalo mínimo entre envios sucessivos (ms)

// --- Enumerações ---
// Define as possíveis direções do joystick
typedef enum {
    CENTRO,     // Joystick em posição neutra
    LESTE,      // Direita
    OESTE,      // Esquerda
    NORTE,      // Cima
    SUL,        // Baixo
    NORDESTE,   // Cima-direita
    NOROESTE,   // Cima-esquerda
    SUDESTE,    // Baixo-direita
    SUDOESTE,   // Baixo-esquerda
    DIRECAO_DESCONHECIDA
} JoystickDirection;

// --- Estruturas ---
// Armazena o estado completo do joystick em um determinado momento
typedef struct {
    JoystickDirection direcao;   // Direção calculada
    int x_position;              // Posição no eixo X (0-100)
    int y_position;              // Posição no eixo Y (0-100)
    uint8_t button_pressed;      // Estado do botão (0=solto, 1=pressionado)
} EstadoJoystick;

// --- Variáveis Globais ---
static EstadoJoystick estado_atual_joystick;     // Estado atual lido do joystick
static EstadoJoystick estado_anterior_joystick;  // Estado anterior para detectar mudanças
static bool wifi_conectado_status = false;       // Status da conexão WiFi
static uint32_t ultimo_envio_dados_ms = 0;       // Timestamp do último envio de dados

// --- Protótipos de Funções Locais ---
static void inicializar_sistema(void);
static bool tentar_conectar_wifi_inicialmente(void); // Renomeado para clareza
static JoystickDirection calcular_direcao_joystick(int x, int y);
static const char* converter_direcao_para_string(JoystickDirection dir);
static bool houve_mudanca_estado_joystick(void);
static void ler_e_processar_joystick(void);
static void tentar_enviar_dados_joystick(void); // Renomeado

// --- Função Principal ---
int main(void) {
    inicializar_sistema();
    printf("Sistema inicializado.\n");

    wifi_conectado_status = tentar_conectar_wifi_inicialmente();

    // Inicializa o estado anterior para forçar o primeiro envio ou ter referência
    memset(&estado_anterior_joystick, 0, sizeof(EstadoJoystick));
    estado_anterior_joystick.direcao = DIRECAO_DESCONHECIDA; // Garante diferença inicial
    estado_anterior_joystick.button_pressed = 2; // Um valor que não seja 0 ou 1

    printf("Iniciando loop principal...\n");
    while (true) {
        cyw43_arch_poll(); // ESSENCIAL!

        ler_e_processar_joystick();
        
        if (wifi_conectado_status) { // Só tenta enviar se a conexão inicial foi bem sucedida
            tentar_enviar_dados_joystick();
        } else {
            // Opcional: adicionar um log periódico se o Wi-Fi não conectou inicialmente
            // Ou tentar reconectar aqui de forma simples após um longo tempo, se desejar,
            // mas vamos começar sem isso.
            static uint32_t ultimo_log_wifi_falhou = 0;
            if (to_ms_since_boot(get_absolute_time()) - ultimo_log_wifi_falhou > 10000) { // A cada 10s
                printf("WiFi não conectado. Não tentando enviar dados.\n");
                // Poderia tentar reconectar aqui:
                // wifi_conectado_status = tentar_conectar_wifi_inicialmente();
                ultimo_log_wifi_falhou = to_ms_since_boot(get_absolute_time());
            }
        }
        sleep_ms(50);
    }
    return 0;
}

// --- Implementações ---

static void inicializar_sistema(void) {
    stdio_init_all();
    sleep_ms(1000);
    joystick_init();
    printf("Joystick inicializado.\n");
}

static bool tentar_conectar_wifi_inicialmente(void) {
    if (conexao_wifi() == 0) { // conexao_wifi() do seu wifi.c atualizado
        printf("WiFi conectado com sucesso!\n");
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(netif_ip4_addr(netif_default)));
        return true;
    } else {
        printf("Falha ao conectar ao WiFi inicialmente.\n");
        return false;
    }
}

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

static bool houve_mudanca_estado_joystick(void) {
    return (estado_atual_joystick.direcao != estado_anterior_joystick.direcao ||
            estado_atual_joystick.button_pressed != estado_anterior_joystick.button_pressed ||
            estado_atual_joystick.x_position != estado_anterior_joystick.x_position || // Ainda comparando X/Y
            estado_atual_joystick.y_position != estado_anterior_joystick.y_position);
}

static void ler_e_processar_joystick(void) {
    Joystick dados_brutos_joystick;
    read_joystick(&dados_brutos_joystick);
    estado_atual_joystick.x_position = dados_brutos_joystick.x_position;
    estado_atual_joystick.y_position = dados_brutos_joystick.y_position;
    estado_atual_joystick.button_pressed = dados_brutos_joystick.button_pressed;
    estado_atual_joystick.direcao = calcular_direcao_joystick(
        estado_atual_joystick.x_position, estado_atual_joystick.y_position);
}

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
            
            // A função enviar_dados_para_nuvem em cliente_http.c tentará resolver DNS, conectar TCP, etc.
            // Se o Wi-Fi tiver caído e se recuperado sozinho (pelo cyw43_arch_poll), isso pode funcionar.
            // Se o Wi-Fi estiver realmente fora, as funções de rede dentro de enviar_dados_para_nuvem
            // devem falhar graciosamente (ex: DNS timeout, TCP connect timeout).
            enviar_dados_para_nuvem(&dados_para_envio);
            ultimo_envio_dados_ms = tempo_atual_ms;
        }
        estado_anterior_joystick = estado_atual_joystick;
    }
}