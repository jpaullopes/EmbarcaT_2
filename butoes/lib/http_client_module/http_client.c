/**
 * @file http_client.c
 * @brief Implementação do cliente HTTP para envio de dados para a nuvem
 *
 * Este arquivo implementa as funções do cliente HTTP que utiliza lwIP para
 * enviar dados dos botões e temperatura para um servidor remoto via HTTP.
 */

#include "cliente_http.h"

/**
 * @brief Callback para receber a resposta do servidor.
 * 
 * Esta função é chamada automaticamente pelo lwIP quando dados são recebidos 
 * do servidor após o envio de uma requisição HTTP.
 *
 * @param arg Argumento passado para o callback (não utilizado)
 * @param pcb PCB da conexão TCP
 * @param p Buffer de dados recebidos
 * @param err Código de erro
 * @return ERR_OK se tudo ocorrer bem, ou um código de erro
 */
static err_t callback_resposta_recebida(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    
    if (!p) {
        printf("Conexão fechada pelo servidor.\n");
        tcp_close(pcb);
        return ERR_OK;
    }

    printf("Resposta do servidor:\n");
    char *dados = (char *)malloc(p->tot_len + 1);
    if (dados) {
        pbuf_copy_partial(p, dados, p->tot_len, 0);
        dados[p->tot_len] = '\0';
        printf("%s\n", dados);
        free(dados);
    }

    pbuf_free(p);
    return ERR_OK;
}

/**
 * @brief Callback para quando a conexão TCP é estabelecida.
 * 
 * Esta função é chamada quando a conexão TCP com o servidor é estabelecida com sucesso.
 * É neste momento que a requisição HTTP POST é montada e enviada para o servidor.
 * 
 * @param arg Argumento passado para o callback (ButtonStates_t*)
 * @param pcb PCB da conexão TCP
 * @param err Código de erro
 * @note É aqui que a requisição HTTP é enviada, após a conexão ser estabelecida.
 *      É necessário usar o PROXY_HOST no cabeçalho Host para funcionar corretamente.
 * @return ERR_OK se tudo ocorrer bem, ou um código de erro
 */
static err_t callback_conectado(void *arg, struct tcp_pcb *pcb, err_t err) {

    ButtonStates_t* estados_completos = (ButtonStates_t*)arg;

    if (err != ERR_OK) {
        printf("Erro ao conectar: %d\n", err);
        tcp_abort(pcb);
        return err;
    }

    tcp_recv(pcb, callback_resposta_recebida);

    char corpo_json[192]; 
    snprintf(corpo_json, sizeof(corpo_json),
             "{\"button_a\": %d, \"button_b\": %d, \"temperature\": %.2f}",
             estados_completos->button_a_pressed ? 1 : 0,
             estados_completos->button_b_pressed ? 1 : 0,
             estados_completos->temperature);
    
    char requisicao[512];
    snprintf(requisicao, sizeof(requisicao),
             "POST /dados HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             PROXY_HOST, strlen(corpo_json), corpo_json);

    cyw43_arch_lwip_begin();
    err_t erro_envio = tcp_write(pcb, requisicao, strlen(requisicao), TCP_WRITE_FLAG_COPY);
    if (erro_envio == ERR_OK) {
        tcp_output(pcb);
        printf("Requisição enviada para %s:%d:\n%s\n", PROXY_HOST, PROXY_PORT, requisicao);
    } else {
        printf("Erro ao enviar dados: %d\n", erro_envio);
        tcp_abort(pcb);
    }
    cyw43_arch_lwip_end();

    return ERR_OK;
}

/**
 * @brief Callback para quando a resolução DNS é concluída.
 * 
 * Esta função é chamada quando o processo de resolução DNS para o nome do host é concluído.
 * Se for bem-sucedido, inicia a conexão TCP para o endereço IP resolvido.
 * 
 * @param nome_host Nome do host que foi resolvido
 * @param ip_resolvido Endereço IP resolvido
 * @param arg Argumento passado para o callback (ButtonStates_t*)
 * @note Se a resolução falhar, imprime uma mensagem de erro. Em caso de sucesso,
 *       ele segue para tentar a conexão TCP.
 */
static void callback_dns_resolvido(const char *nome_host, const ip_addr_t *ip_resolvido, void *arg) {
    
    ButtonStates_t* dados_recebidos = (ButtonStates_t*)arg;

    if (!ip_resolvido) {
        printf("Erro: DNS falhou para %s\n", nome_host);
        return;
    }

    printf("DNS resolveu %s para %s\n", nome_host, ipaddr_ntoa(ip_resolvido));

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) {
        printf("Erro ao criar pcb\n");
        return;
    }

    tcp_arg(pcb, arg);

    // Conectar à porta do PROXY
    err_t erro = tcp_connect(pcb, ip_resolvido, PROXY_PORT, callback_conectado);
    if (erro != ERR_OK) {
        printf("Erro ao conectar a %s:%d: %d\n", nome_host, PROXY_PORT, erro);
        tcp_abort(pcb);
    }
}

/**
 * @brief Envia os dados do ButtonStates_t para o servidor na nuvem.
 * 
 * Esta função inicia o processo de envio de dados para a nuvem.
 * Primeiro, ela tenta resolver o nome de domínio do servidor por DNS.
 * Dependendo do resultado, pode proceder diretamente com a conexão TCP
 * ou aguardar a resolução DNS assíncrona.
 * 
 * @param dados_a_enviar Ponteiro para a estrutura ButtonStates_t com os dados a enviar
 * @note Usar PROXY_HOST para resolução DNS. Assim que o DNS for resolvido, 
 *       a conexão TCP é estabelecida e os dados são enviados.
 */
void enviar_dados_para_nuvem(const ButtonStates_t* dados_a_enviar) {
    ip_addr_t endereco_ip;
    // Usar PROXY_HOST para resolução DNS
    err_t resultado_dns = dns_gethostbyname(PROXY_HOST, &endereco_ip, callback_dns_resolvido, (void*)dados_a_enviar);

    if (resultado_dns == ERR_OK) {
        // Se já resolvido (cache), conectar diretamente à porta do PROXY
        printf("Host %s já resolvido para %s\n", PROXY_HOST, ipaddr_ntoa(&endereco_ip));

        struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
        if (!pcb) {
            printf("Erro ao criar pcb (cache)\n");
            return;
        }
        
        tcp_arg(pcb, (void*)dados_a_enviar);

        // Conectar à porta do PROXY
        err_t erro = tcp_connect(pcb, &endereco_ip, PROXY_PORT, callback_conectado);
        if (erro != ERR_OK) {
            printf("Erro ao conectar (cache) a %s:%d: %d\n", PROXY_HOST, PROXY_PORT, erro);
            tcp_abort(pcb);
        }
    } else if (resultado_dns == ERR_INPROGRESS) {
        printf("Resolução DNS em andamento para %s...\n", PROXY_HOST);
    } else {
        printf("Erro ao iniciar DNS para %s: %d\n", PROXY_HOST, resultado_dns);
    }
}