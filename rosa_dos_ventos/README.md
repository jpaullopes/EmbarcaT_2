# 🌐 Rosa dos Ventos

> Sistema embarcado para leitura de joystick e envio de dados via Wi-Fi (Raspberry Pi Pico + CYW43).

## 📋 Sumário

- [🔎 Descrição](#-descrição)
- [🚀 Funcionalidades](#-funcionalidades)
- [🧩 Estrutura do Projeto](#-estrutura-do-projeto)
- [📦 Pré-requisitos](#-pré-requisitos)
- [⚙️ Configuração e Build](#️-configuração-e-build)
- [⚡ Uso](#-uso)
- [📁 Detalhamento das Pastas](#-detalhamento-das-pastas)
- [🛠 Módulos Principais](#-módulos-principais)
- [🤝 Contribuições](#-contribuições)
- [📄 Licença](#-licença)

---

## 🔎 Descrição

Este projeto implementa um sistema embarcado baseado em **Raspberry Pi Pico** (RP2040) que:

- Lê a posição de um joystick analógico (e o estado do botão) via driver ADC.
- Converte posições X/Y em direções (Norte, Sul, Leste, Oeste, diagonais e centro).
- Monitora mudanças de estado e imprime logs via `stdio`.
- Envia dados para a nuvem através de uma requisição HTTP usando o módulo Wi-Fi CYW43.

Ideal para aplicações de controle remoto, monitoramento e prototipagem rápida.

## 🚀 Funcionalidades

- ✔️ Leitura de joystick com dead zone configurável.
- ✔️ Cálculo de direções cardinal e intercardinal.
- ✔️ Conexão Wi-Fi automática (módulo CYW43).
- ✔️ Envio de dados `Joystick` via HTTP (GET/POST).
- ✔️ Logs detalhados de estado e eventos.

## 🧩 Estrutura do Projeto

```text
rosa_dos_ventos/
├── CMakeLists.txt           # Configuração do projeto Pico SDK
├── pico_sdk_import.cmake    # Import do SDK
├── src/
│   └── app_main.c           # `main()` e loop principal
├── lib/
│   ├── joystick_driver/     # Driver de leitura ADC do joystick
│   ├── wifi_module/         # Conexão e gestão Wi-Fi
│   └── http_client_module/  # Cliente HTTP para envio de dados
├── config/                  # Configurações LPC e FreeRTOS
├── build/                   # Artefatos de compilação (ninja, ELF, UF2)
└── README.md                # Este arquivo
```

## 📦 Pré-requisitos

- **Hardware**:
  - Raspberry Pi Pico (com CYW43 para Wi-Fi) ou variante Pico W.
  - Joystick analógico (potenciómetros X/Y) + botão.
  - Cabos e fonte 5V.

- **Software**:
  - Toolchain ARM GCC para RP2040.
  - [Pico SDK](https://github.com/raspberrypi/pico-sdk).
  - Ninja build.
  - OpenOCD (para flash via CMSIS-DAP ou Picotool).

## ⚙️ Configuração e Build

1. Clone o repositório:
   ```powershell
   git clone https://<seu-repo>/rosa_dos_ventos.git
   cd rosa_dos_ventos
   ```

2. Inicialize submódulos / SDK (se aplicável):
   ```powershell
   git submodule update --init --recursive
   ```

3. Crie pasta de build e compile:
   ```powershell
   mkdir build; cd build
   cmake -G Ninja ..
   ninja
   ```

4. Gere UF2/ELF:
   - O artefato `joystick.uf2` estará em `build/`.

5. Flash no Pico W (arraste e solte UF2):
   - Conecte o Pico em modo `BOOTSEL`.
   - Copie `joystick.uf2` para o drive virtual.

> **Dica**: Você pode usar o Picotool via VSCode Tasks:
> - **Compile Project** - compila o projeto.
> - **Flash** - faz deploy pelo OpenOCD.

## ⚡ Uso

1. Abra o monitor serial (115200 bps).
2. O sistema inicializa e conecta no Wi-Fi:
   ```plain
   Joystick inicializado.
   WiFi conectado com sucesso!
   IP do dispositivo: 192.168.xx.xx
   ```
3. Mova o joystick ou pressione o botão, veja logs:
   ```plain
   Mudança Joystick: X=85, Y=42, Btn=0, Dir=Sudoeste
   Enviando dados para a nuvem...
   ```
4. No servidor HTTP, receba/processa os dados do JSON.

## 📁 Detalhamento das Pastas

- **src/**
  - `app_main.c`: lógica principal de leitura/processamento e envio.

- **lib/joystick_driver/**
  - `joystick.c/.h`: inicialização e leitura analógica.

- **lib/wifi_module/**
  - `wifi.c/.h`: gerenciamento de conexão via CYW43.

- **lib/http_client_module/**
  - `cliente_http.c/.h`: funções para requisições HTTP.

- **config/**
  - `FreeRTOSConfig.h`, `lwipopts.h`: configurações de RTOS e rede.

- **build/**
  - Artefatos de compilação gerados pelo CMake/Ninja.

## 🛠 Módulos Principais

| Módulo                  | Responsabilidade                                |
|-------------------------|-------------------------------------------------|
| joystick_driver         | Leitura de X/Y e botão do joystick via ADC      |
| wifi_module             | Conexão e manutenção de link Wi-Fi              |
| http_client_module      | Envio de dados via HTTP (DNS, TCP, TIMEOUTs)    |
| app_main               | Estado, lógica de envio e debounce de rede      |

## 🤝 Contribuições

Contribuições são bem-vindas:

1. Fork no GitHub.
2. Crie uma branch: `feature/minha-nova-funcionalidade`.
3. Commit suas mudanças e abra um PR.
4. Aguarde revisão e merge.

## 📄 Licença

Este projeto está licenciado sob a **MIT License**. Veja [LICENSE](LICENSE) para mais detalhes.
