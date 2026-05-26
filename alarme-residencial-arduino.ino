// Bibliotecas utilizadas
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

// Definição dos pinos dos componentes
#define buzzer 3
#define led 2
#define pir 5
#define resetButton 4

// Configuração do Teclado Matricial 4x4
const byte QTD_LINHAS = 4;
const byte QTD_COLUNAS = 4;
char matriz_teclas[QTD_LINHAS][QTD_COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Pinos conectados às Linhas (13, 12, 11, 10) e Colunas (9, 8, 7, 6) conforme a imagem
byte pinosLinhas[QTD_LINHAS] = {13, 12, 11, 10};
byte pinosColunas[QTD_COLUNAS] = {9, 8, 7, 6};

// Cria o objeto do teclado matricial utilizando a biblioteca Keypad
Keypad teclado = Keypad(makeKeymap(matriz_teclas), pinosLinhas, pinosColunas, QTD_LINHAS, QTD_COLUNAS);

// Configuração do LCD I2C (Endereço 0 padrão do Tinkercad/Adafruit)
Adafruit_LiquidCrystal lcd_1(0);

// Definição dos Estados do Sistema (Máquina de Estados)
enum Estados { DESARMADO, ARMADO, DISPARADO };
Estados estadoAtual = DESARMADO;

// Variáveis de controle de senha
const String PIN_CORRETO = "1234"; // Conforme exigido no requisito 
String pinDigitado = "";

// Variável para controle de tempo do LED piscando sem travar o código (millis)
unsigned long tempoAnteriorLED = 0;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(resetButton, INPUT); 
  pinMode(pir, INPUT);
  
  lcd_1.begin(16, 2);
  lcd_1.setBacklight(1);
  
  exibirMenuInicial();
}

void loop() {
  char tecla = teclado.getKey();

  switch (estadoAtual) {
    
    // ==========================================
    // BLOCO 1: ESTADO DESARMADO
    // ==========================================
    case DESARMADO:
      digitalWrite(led, LOW);
      noTone(buzzer);
      
      if (tecla) {
        // Se pressionar '*', limpa a digitação
        if (tecla == '*') {
          pinDigitado = "";
          exibirMenuInicial();
        } 
        // Se pressionar '#', confirma a senha
        else if (tecla == '#') {
          if (pinDigitado == PIN_CORRETO) {
            lcd_1.clear();
            lcd_1.print("PIN Correto!");
            lcd_1.setCursor(0, 1);
            lcd_1.print("SISTEMA ARMADO");
            delay(2000);
            
            pinDigitado = "";
            estadoAtual = ARMADO;
            exibirMenuArmado();
          } else {
            lcd_1.clear();
            lcd_1.print("PIN Incorreto!");
            delay(2000);
            pinDigitado = "";
            exibirMenuInicial();
          }
        } 
        // Adiciona os números digitados (limite de 4 dígitos)
        else if (pinDigitado.length() < 4 && tecla != 'A' && tecla != 'B' && tecla != 'C' && tecla != 'D') {
          pinDigitado += tecla;
          lcd_1.setCursor(8 + pinDigitado.length() - 1, 1);
          lcd_1.print("*"); // Mascara a senha digitada 
        }
      }
      break;

    // ==========================================
    // BLOCO 2: ESTADO ARMADO (MONITORANDO)
    // ==========================================
    case ARMADO:
      // Verifica se o sensor PIR detectou presença 
      if (digitalRead(pir) == HIGH) {
        estadoAtual = DISPARADO;
        exibirMenuDisparado();
      }
      
      // Permite desarmar inserindo o código PIN novamente 
      if (tecla) {
        if (tecla == '*') {
          pinDigitado = "";
          exibirMenuArmado();
        } 
        else if (tecla == '#') {
          if (pinDigitado == PIN_CORRETO) {
            lcd_1.clear();
            lcd_1.print("Alarme");
            lcd_1.setCursor(0, 1);
            lcd_1.print("Desarmado!");
            delay(2000);
            pinDigitado = "";
            estadoAtual = DESARMADO;
            exibirMenuInicial();
          } else {
            pinDigitado = "";
            exibirMenuArmado();
          }
        } 
        else if (pinDigitado.length() < 4 && tecla != 'A' && tecla != 'B' && tecla != 'C' && tecla != 'D') {
          pinDigitado += tecla;
          lcd_1.setCursor(8 + pinDigitado.length() - 1, 1); // Posiciona após o texto "Senha: "
          lcd_1.print("*");
        }
      }
      break;

    // ==========================================
    // BLOCO 3: ESTADO DISPARADO
    // ==========================================
    case DISPARADO:
      // Executa o alarme a cada 150 ms sem bloquear o programa 
      unsigned long tempoAtual = millis();
      if (tempoAtual - tempoAnteriorLED >= 150) {
        tempoAnteriorLED = tempoAtual;
        digitalWrite(led, HIGH);
        tone(buzzer, 600);
      }

      // Verifica se o Botão de Reset foi pressionado  
      if (digitalRead(resetButton) == HIGH) {
        noTone(buzzer);
        digitalWrite(led, LOW);
        lcd_1.clear();
        lcd_1.print("Alarme Resetado");
        delay(2000);
        
        pinDigitado = "";
        estadoAtual = DESARMADO; // Retorna ao estado de espera inicial 
        exibirMenuInicial();
      }
      break;
  }
}

// ==========================================
// FUNÇÕES AUXILIARES DE INTERFACE (LCD)
// ==========================================
void exibirMenuInicial() {
  lcd_1.clear();
  lcd_1.print("1-DESARMADO"); 
  lcd_1.setCursor(0, 1);
  lcd_1.print("Senha: "); 
}

void exibirMenuArmado() {
  lcd_1.clear();
  lcd_1.print("2-ARMADO [OK]");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Senha: ");
}

void exibirMenuDisparado() {
  lcd_1.clear();
  lcd_1.print("!! ALERTA !!"); 
  lcd_1.setCursor(0, 1);
  lcd_1.print("INTRUSO DETECTADO"); 
}
