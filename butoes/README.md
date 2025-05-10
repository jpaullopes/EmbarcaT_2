<p align="center">
  <img src="https://www.raspberrypi.com/documentation/computers/images/pico_w_board.png" alt="Raspberry Pi Pico W" width="200"/>
</p>

<h1 align="center">🔘 Sistema de Botões IoT<br>com FreeRTOS e Wi-Fi</h1>

<p align="center">
  <b>Automação, prototipagem e IoT de forma simples e robusta!</b>
</p>

---

## ✨ Visão Geral

Este projeto implementa um sistema embarcado para leitura de botões físicos, utilizando o microcontrolador <b>Raspberry Pi Pico W</b>, com <b>FreeRTOS</b>, conectividade Wi-Fi e envio dos estados dos botões para a nuvem via HTTP. O sistema é modular, robusto e ideal para aplicações de automação, prototipagem e IoT.

---

## 📋 Sumário

- [Funcionalidades](#funcionalidades)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Hardware Utilizado](#hardware-utilizado)
- [Dependências](#dependências)
- [Como Funciona](#como-funciona)
- [Envio de Dados](#envio-de-dados)
- [Como Compilar](#como-compilar)
- [Como Gravar na Placa](#como-gravar-na-placa)
- [Configurações Importantes](#configurações-importantes)
- [Autores](#autores)
- [Referências](#referências)
- [Licença](#licença)

---

## ⚙️ Funcionalidades

- 🔘 Leitura de dois botões físicos (A e B) via GPIO
- 🛡️ Debouncing por software
- ☁️ Envio periódico do estado dos botões para a nuvem (HTTP POST)
- 📶 Conexão automática à rede Wi-Fi
- 🔄 Reconexão automática em caso de falha
- 🧩 Arquitetura baseada em FreeRTOS (multitarefa)
- 📬 Comunicação entre tasks via fila (Queue)
- 🖨️ Logs detalhados via USB

---

## 🗂️ Estrutura do Projeto

```text
butoes/
├── src/
│   └── app_main.c                # Lógica principal, tasks e orquestração
├── lib/
│   ├── buttons_driver/
│   │   ├── buttons.c             # Driver dos botões (GPIO)
│   │   └── buttons.h
│   ├── http_client_module/
│   │   ├── http_client.c         # Cliente HTTP para envio dos dados
│   │   └── cliente_http.h
│   └── wifi_module/
│       ├── wifi.c                # Gerenciamento de conexão Wi-Fi
│       └── wifi.h
├── config/
│   └── FreeRTOSConfig.h          # Configurações do FreeRTOS
├── CMakeLists.txt                # Build system (CMake)
└── ...
```

---

## 🛠️ Hardware Utilizado

- <img src="https://www.raspberrypi.com/documentation/computers/images/pico_w_board.png" width="24"/> **Placa:** Raspberry Pi Pico W
- 🔘 **Botões:** 2 botões físicos conectados aos pinos GPIO 5 (A) e 6 (B)
- 📡 **Conexão:** Wi-Fi 2.4GHz

---

## 📦 Dependências

- Raspberry Pi Pico SDK (>= 1.5.0)
- FreeRTOS Kernel (porta para RP2040)
- lwIP (TCP/IP stack)
- CMake (>= 3.13)
- Compilador ARM (arm-none-eabi)
- Ninja (opcional, recomendado)

---

## 🚦 Como Funciona

1. <b>Inicialização:</b>  
   O sistema inicializa o FreeRTOS, configura os GPIOs dos botões e tenta conectar ao Wi-Fi.

2. <b>Leitura dos Botões:</b>  
   Uma task (`button_task`) lê periodicamente o estado dos botões e envia mudanças para uma fila.

3. <b>Envio para a Nuvem:</b>  
   Outra task (`wifi_task`) recebe os estados da fila e, se conectado ao Wi-Fi, envia os dados para a nuvem via HTTP POST (JSON).

4. <b>Reconexão:</b>  
   Se o Wi-Fi cair, o sistema tenta reconectar automaticamente.

---

## 🌐 Envio de Dados

- **Formato:** JSON  
  Exemplo:
  ```json
  {
    "button_a": 1,
    "button_b": 0
  }
  ```
- **Endpoint:**  
  Os dados são enviados via proxy HTTP definido em `cliente_http.h`:
  - Host: `crossover.proxy.rlwy.net`
  - Porta: `12011`
  - Caminho: `/dados`

---

## 🧑‍💻 Como Compilar

```sh
# Clone o repositório e entre na pasta
$ git clone <repo-url>
$ cd butoes

# Configure o ambiente do Pico SDK e FreeRTOS
# (Certifique-se de que as variáveis de ambiente estejam corretas)

# Compile o projeto
$ mkdir build
$ cd build
$ cmake ..
$ ninja
```
Ou use o comando de build do VS Code.

---

## ⚙️ Configurações Importantes

- **Nome e senha do Wi-Fi:**  
  Definidos em `lib/wifi_module/wifi.h`  
  ```c
  #define NOME_REDE_WIFI "SeuWiFi"
  #define SENHA_REDE_WIFI "SuaSenha"
  ```

- **Pinos dos botões:**  
  Definidos em `lib/buttons_driver/buttons.h`  
  ```c
  #define BUTTON_A_PIN 5
  #define BUTTON_B_PIN 6
  ```

---

## 👨‍💻 Autores

- Projeto desenvolvido por João Paulo Lopes
- Baseado em exemplos do Pico SDK, FreeRTOS, lwIP e amigos
 
---

## 📚 Referências

- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [FreeRTOS](https://www.freertos.org/)
- [lwIP](https://savannah.nongnu.org/projects/lwip/)
- [Documentação oficial do Pico W](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)

---

## 📝 Licença

Este projeto é open-source e pode ser utilizado para fins educacionais e de prototipagem.

---

<p align="center">
  <img src="https://img.shields.io/badge/Feito%20com-FreeRTOS-blue?style=for-the-badge&logo=freertos"/>
  <img src="https://img.shields.io/badge/Hardware-Pico%20W-green?style=for-the-badge&logo=raspberrypi"/>
  <img src="https://img.shields.io/badge/IoT-Cloud-blueviolet?style=for-the-badge&logo=cloudflare"/>
</p>
