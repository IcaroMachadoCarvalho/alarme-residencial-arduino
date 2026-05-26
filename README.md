# Sistema de Alarme Residencial Inteligente (Edge Computing)

Este projeto consiste em um **Sistema de Alarme Residencial** microcontrolado utilizando a plataforma Arduino Uno. O sistema é capaz de monitorar ambientes contra intrusões por meio de sensores de presença, gerenciar o arme e desarme por meio de senha em um teclado matricial, emitir alertas visuais/sonoros e fornecer uma interface clara ao usuário por meio de um display LCD.

---

## 🛠️ Arquitetura do Sistema

O projeto foi projetado utilizando a abordagem de **Sistemas Embarcados com Edge Computing**, processando todas as tomadas de decisão e regras de negócio localmente no próprio microcontrolador, garantindo tempo de resposta imediato (tempo real).

### Componentes Utilizados
* **Microcontrolador:** Arduino Uno R3
* **Sensor de Presença:** Sensor PIR (Passive Infrared) – responsável pela detecção de movimento.
* **Teclado de Controle:** Teclado Matricial 4x4 – interface para inserção do código PIN.
* **Display de Interface:** LCD 16x2 com Módulo de Comunicação I2C.
* **Atuador Sonoro:** Buzzer Piezoelétrico.
* **Atuador Visual:** LED Difuso Vermelho + Resistor de 220Ω.
* **Controle Manual:** Chave Táctil (Push Button) operando em modo `INPUT`.

### Esquema de Conexões (Pinagem)

| Componente | Pino Arduino | Tipo de Sinal | Função |
| :--- | :---: | :---: | :--- |
| **LED (Alarme)** | `D2` | Saída Digital | Alerta visual de intrusão |
| **Buzzer** | `D3` | Saída PWM/Digital | Sirene sonora de intrusão |
| **Botão de Reset** | `D4` | Entrada Digital | Reset manual do sistema disparado |
| **Sensor PIR** | `D5` | Entrada Digital | Detector de presença/movimento |
| **Teclado (Linhas)** | `D13, D12, D11, D10` | Entrada/Saída | Varredura de linhas da matriz |
| **Teclado (Colunas)**| `D9, D8, D7, D6` | Entrada/Saída | Varredura de colunas da matriz |
| **Display LCD (SDA)**| `A4 / SDA` | Comunicação I2C| Barramento de dados do display |
| **Display LCD (SCL)**| `A5 / SCL` | Comunicação I2C| Barramento de clock do display |

---

## 🧠 Lógica de Funcionamento e Máquina de Estados

Para garantir estabilidade, confiabilidade e evitar o uso de funções bloqueantes (como `delay()`), o software foi estruturado como uma **Máquina de Estados Finitos (FSM)**. O sistema alterna entre três estados principais bem definidos:

### 1. Estado: `DESARMADO` (Aguardando Ativação)
* **Comportamento:** O sistema fica em repouso. Os atuadores (LED e Buzzer) permanecem desligados.
* **Interface LCD:** Exibe `1-DESARMADO` e solicita a inserção da senha. Conforme os dígitos são inseridos, caracteres `*` mascaram o PIN na tela por segurança.
* **Transição de Estado:** * Se o usuário digitar o PIN correto (`1234`) e pressionar `#`, o sistema limpa a memória temporária e avança para o estado `ARMADO`.
  * Se a tecla `*` for pressionada, limpa a digitação atual.

### 2. Estado: `ARMADO` (Monitoramento Ativo)
* **Comportamento:** O perímetro está protegido. O Arduino passa a ler continuamente o pino do Sensor PIR.
* **Interface LCD:** Exibe `2-ARMADO [OK]` e permite que o usuário insira a senha caso queira desarmar o sistema manualmente de forma pacífica.
* **Transição de Estado:**
  * **Se o Sensor PIR detectar movimento (`HIGH`):** O sistema muda imediatamente para o estado `DISPARADO`.
  * **Se o PIN correto for digitado:** O sistema retorna para o estado `DESARMADO`.

### 3. Estado: `DISPARADO` (Invasão Detectada)
* **Comportamento:** O alarme foi violado. Uma sirene de dupla frequência (alternando entre 600Hz e 1000Hz) é emitida pelo Buzzer, enquanto o LED pisca rapidamente em intervalos de 150ms utilizando a função `millis()`.
* **Interface LCD:** Exibe alertas piscantes de `!! ALERTA !! / INTRUSO DETECTADO`.
* **Transição de Estado:** * O sistema ignora novas detecções do PIR e se concentra em monitorar o Botão de Reset. Assim que o botão físico de Reset for pressionado, os atuadores são desligados e o sistema regressa com segurança para o estado inicial `DESARMADO`.

---

## 💻 Estrutura do Código-Fonte

O código foi otimizado dividindo-se em seções modulares:
* **Configuração de Bibliotecas:** Inclusão das bibliotecas `<Keypad.h>` para a varredura matricial e `<Adafruit_LiquidCrystal.h>` para o barramento I2C.
* **Diretivas de Pré-processamento:** Uso de `#define` para mapeamento de hardware, otimizando o uso de memória Flash.
* **Bloco `switch-case` Principal:** Centraliza o fluxo de controle no `loop()`, garantindo o isolamento das ações e legibilidade do código.
* **Funções Auxiliares de Atualização:** Funções exclusivas para atualizar o display LCD (`exibirMenuInicial`, `exibirMenuArmado`, etc.), evitando flickering (piscadas indesejadas na tela).

---

## 🚀 Como Executar o Projeto

1. Acesse o ambiente do **Tinkercad**.
2. Execute a simulação ou faça uma cópia para sua conta
3. Copie o código fonte contido no arquivo `alarme-residencial-arduino.ino` (ou fornecido na entrega) e cole no editor do simulador.
4. Inicie a simulação.
5. **Teste de Arme:** Digite `1234#` no teclado. O sistema passará para o modo ARMADO.
6. **Teste de Disparo:** Clique sobre o Sensor PIR e simule um movimento. O alarme disparará (LED e Som ativos).
7. **Teste de Reset:** Clique no botão táctil (Reset) para desativar o alarme e voltar ao início.
