#include <DHT.h>
#include <math.h>

// =====================================================
// FarmTech Nexo - Irrigacao Inteligente
// Cultura simulada: Tomate
//
// DHT22: umidade do solo simulada
// LDR: pH do solo simulado
// Botoes: Nitrogenio, Fosforo e Potassio
// Rele: bomba de irrigacao
// =====================================================

// -------------------- PINOS -------------------------

const int PINO_DHT = 4;
const int TIPO_DHT = DHT22;

const int PINO_LDR = 34;

const int PINO_N = 14;
const int PINO_P = 27;
const int PINO_K = 26;

const int PINO_RELE = 25;

// ---------------- CONFIGURACOES ---------------------

const float UMIDADE_MINIMA = 60.0;

const float PH_MINIMO = 6.0;
const float PH_MAXIMO = 6.8;

// Modulo de rele Wokwi: normalmente ativo em LOW.
const int RELE_LIGADO = LOW;
const int RELE_DESLIGADO = HIGH;

const unsigned long INTERVALO_LEITURA = 2500;

DHT dht(
  PINO_DHT,
  TIPO_DHT
);

// ------------------- FUNCOES ------------------------

float converterLdrParaPh(int leituraLdr) {
  return (leituraLdr * 14.0) / 4095.0;
}

void ligarBomba() {
  digitalWrite(
    PINO_RELE,
    RELE_LIGADO
  );
}

void desligarBomba() {
  digitalWrite(
    PINO_RELE,
    RELE_DESLIGADO
  );
}

void imprimirNutriente(
  const char* nome,
  bool presente
) {
  Serial.print(nome);
  Serial.print(": ");
  Serial.println(
    presente ? "PRESENTE" : "AUSENTE"
  );
}

// -------------------- SETUP --------------------------

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(PINO_N, INPUT_PULLUP);
  pinMode(PINO_P, INPUT_PULLUP);
  pinMode(PINO_K, INPUT_PULLUP);

  pinMode(PINO_RELE, OUTPUT);

  desligarBomba();

  Serial.println();
  Serial.println("========================================");
  Serial.println("FarmTech Nexo - Irrigacao Inteligente");
  Serial.println("Cultura simulada: Tomate");
  Serial.println("Sistema iniciado.");
  Serial.println("========================================");
}

// --------------------- LOOP --------------------------

void loop() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  int leituraLdr = analogRead(PINO_LDR);

  float phSimulado =
    converterLdrParaPh(leituraLdr);

  bool nitrogenioPresente =
    digitalRead(PINO_N) == LOW;

  bool fosforoPresente =
    digitalRead(PINO_P) == LOW;

  bool potassioPresente =
    digitalRead(PINO_K) == LOW;

  bool leituraDhtValida =
    !isnan(umidade)
    && !isnan(temperatura);

  bool nutrientesAdequados =
    nitrogenioPresente
    && fosforoPresente
    && potassioPresente;

  bool phAdequado =
    phSimulado >= PH_MINIMO
    && phSimulado <= PH_MAXIMO;

  bool soloSeco =
    leituraDhtValida
    && umidade < UMIDADE_MINIMA;

  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println("LEITURA DA LAVOURA");

  if (!leituraDhtValida) {
    Serial.println(
      "ERRO: leitura invalida do DHT22."
    );

    Serial.println(
      "Confira DHT22: VCC -> 3V3, SDA -> GPIO 4, GND -> GND."
    );

    Serial.println(
      "BOMBA: DESLIGADA por seguranca."
    );

    desligarBomba();

    Serial.println("----------------------------------------");

    delay(INTERVALO_LEITURA);
    return;
  }

  Serial.print("Umidade do solo simulada: ");
  Serial.print(umidade, 1);
  Serial.println("%");

  Serial.print("Temperatura ambiente: ");
  Serial.print(temperatura, 1);
  Serial.println(" C");

  Serial.print("LDR: ");
  Serial.println(leituraLdr);

  Serial.print("pH simulado: ");
  Serial.println(phSimulado, 2);

  imprimirNutriente(
    "Nitrogenio (N)",
    nitrogenioPresente
  );

  imprimirNutriente(
    "Fosforo (P)",
    fosforoPresente
  );

  imprimirNutriente(
    "Potassio (K)",
    potassioPresente
  );

  Serial.println("----------------------------------------");

  if (!nutrientesAdequados) {
    desligarBomba();

    Serial.println("BOMBA: DESLIGADA");
    Serial.println(
      "Motivo: NPK incompleto."
    );
  }

  else if (!phAdequado) {
    desligarBomba();

    Serial.println("BOMBA: DESLIGADA");
    Serial.println(
      "Motivo: pH simulado fora da faixa ideal."
    );
  }

  else if (soloSeco) {
    ligarBomba();

    Serial.println("BOMBA: LIGADA");
    Serial.println(
      "Motivo: solo seco, NPK adequado e pH correto."
    );
  }

  else {
    desligarBomba();

    Serial.println("BOMBA: DESLIGADA");
    Serial.println(
      "Motivo: umidade do solo suficiente."
    );
  }

  Serial.println("----------------------------------------");

  delay(INTERVALO_LEITURA);
}