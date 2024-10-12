#include <Keypad.h>
#include <LiquidCrystal.h>
#include <string.h> // Necessário para strcmp

// tempo para desarmar a bomba
int seconds = 300;
unsigned long previousMillis = 0; // Armazena o último tempo em que o temporizador foi atualizado
unsigned long previousToneMillis = 0; // Armazena o último tempo em que o som foi tocado
long interval = 1000;       // Intervalo de 1 segundo (1000 ms)
long toneInterval = 1500; // Intervalo do som, será ajustado
const int initialSeconds = 300;    // Valor inicial do cronômetro

// Defina os pinos do LED e do Buzzer
const int tonePin = 13; // Pino do Buzzer
const int ledPin = 12;  // Pino do LED

// Defina os pinos do LCD usando entradas analógicas
const int lcdRS = A0;
const int lcdE = A1;
const int lcdD4 = A2;
const int lcdD5 = A3;
const int lcdD6 = A4;
const int lcdD7 = A5;

// Crie o objeto para o LCD
LiquidCrystal lcd(lcdRS, lcdE, lcdD4, lcdD5, lcdD6, lcdD7);

// Frequências das notas Mi e Fá
const int toneE = 659; // Frequência do Mi (E) em Hz
const int toneF = 698; // Frequência do Fá (F) em Hz

// Defina o número de linhas e colunas do teclado
const byte numRows = 4;
const byte numCols = 4;

// Defina os pinos conectados às linhas e colunas do teclado
byte rowPins[numRows] = {5, 6, 7, 8}; // Pinos conectados às linhas do teclado
byte colPins[numCols] = {9, 2, 3, 4}; // Pinos conectados às colunas do teclado

// Defina os caracteres para cada botão do teclado
char keys[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Crie o objeto do teclado
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, numRows, numCols);

// Variável para armazenar a entrada do usuário (4 dígitos)
char inputCode[5] = "";  // 4 dígitos mais o terminador nulo '\0'
int currentIndex = 0;    // Índice para armazenar a posição atual no array

// Array de senhas corretas
const char correctCodes[10][5] = {
  "8392", "1749", "5217", "6948", "8306",
  "1954", "3861", "4729", "9185", "3067"
};

// Função para verificar o código
void checkCode() {
  bool codeMatch = false;

  // Verifica se o código digitado corresponde a algum dos códigos válidos
  for (int i = 0; i < 10; i++) {
    if (strcmp(inputCode, correctCodes[i]) == 0) {
      codeMatch = true;
      break;
    }
  }

  if (codeMatch) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bomba Desarmada");
    digitalWrite(ledPin, LOW);  // Desliga o LED
    noTone(tonePin);            // Para o som do buzzer
    delay(5000);
    resetGame();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Codigo Errado!");
    lcd.setCursor(0, 1);
    delay(2000);
    clearCode();
    restoreInterface();
  }
}

// Função para restaurar a interface com o cronômetro e o campo de código
void restoreInterface() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMPO: ");
  lcd.print(seconds);
  lcd.print("s");

  lcd.setCursor(0, 1);
  lcd.print("Codigo: ");
  lcd.setCursor(7, 1);    // Posiciona no início da área dos dígitos
  lcd.print(inputCode);
}

// Função para apagar a senha inteira
void clearCode() {
  currentIndex = 0;
  inputCode[0] = '\0';
  lcd.setCursor(0, 1);
  lcd.print("Codigo:    ");
}

// Função para apagar o último número (dígito por dígito)
void deleteLastDigit() {
  if (currentIndex > 0) {
    currentIndex--;
    inputCode[currentIndex] = '\0';
    lcd.setCursor(0, 1);
    lcd.print("Codigo: ");
    lcd.print(inputCode);
    lcd.print(" ");
  }
}

// Função para acelerar o cronômetro
void velTimer() {
  if (interval > 250) {
    interval /= 3;
    lcd.setCursor(0, 1);
    lcd.print("Cron. acelerado");
  }
}

// Função para resetar o jogo
void resetGame() {
  seconds = initialSeconds;
  currentIndex = 0;
  inputCode[0] = '\0';
  interval = 1000;
  toneInterval = 1500;
  startGame();
}

// Função para iniciar o jogo
void startGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pressione tecla");
  lcd.setCursor(0, 1);
  lcd.print("para iniciar!");

  while (true) {
    char key = keypad.getKey();
    if (key) {
      restoreInterface();
      break;
    }
  }
}

void setup() {
  lcd.begin(16, 2); // Inicializa o LCD 16x2
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  startGame();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == 'D') {
      checkCode();
    } else if (key == 'A') {
      clearCode();
    } else if (key == 'B') {
      velTimer();
    } else if (key == 'C') {
      deleteLastDigit();
    } else if (currentIndex < 4) {
      inputCode[currentIndex] = key;
      currentIndex++;
      inputCode[currentIndex] = '\0';
      restoreInterface();
    }
  }

  unsigned long currentMillis = millis();

  // Controle do cronômetro
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (seconds <= 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BOOM!");
      digitalWrite(ledPin, HIGH);
      tone(tonePin, toneE);
      delay(5000);
      digitalWrite(ledPin, LOW);
      noTone(tonePin);
      resetGame();
    }

    lcd.setCursor(0, 0);
    lcd.print("TEMPO: ");
    lcd.print(seconds);
    lcd.print("s ");
    seconds--;
  }

  // Controle do som e do LED enquanto o tempo está ativo
  if (currentMillis - previousToneMillis >= toneInterval) {
    previousToneMillis = currentMillis;

    // Ajusta a velocidade conforme o tempo restante
    if (seconds <= 15) {
      toneInterval = 150; // Muito rápido nos últimos 15 segundos
    }

    // Alterna entre Mi e Fá se faltam menos de 60 segundos
    tone(tonePin, (previousToneMillis % 2 == 0) ? toneE : toneF, 100);
    digitalWrite(ledPin, (previousToneMillis % 2 == 0) ? HIGH : LOW);
  }
}
