
 // =====================================================
// VIACIDADA - PROTOTIPO IOT
// Monitoramento de presenca de transporte
// =====================================================

// IDENTIFICACAO DA INSTALACAO
const char* ID_DISPOSITIVO = "VC-001";
const char* NOME_PARADA = "Terminal Santana";
const char* TIPO_TRANSPORTE = "ONIBUS";

// PINOS DO SENSOR ULTRASSONICO
const int PINO_TRIG = 2;
const int PINO_ECHO = 3;

// PINOS DOS LEDS
const int LED_VERMELHO = 6;
const int LED_VERDE = 7;

// CONFIGURACOES
const float DISTANCIA_LIMITE = 250.0;
const unsigned long INTERVALO_ATUALIZACAO = 60000;

// CONTROLE DE ESTADO
bool estadoAnterior = false;
bool primeiraLeitura = true;

unsigned long ultimaAtualizacao = 0;

// =====================================================
// FUNCAO DE MEDICAO
// =====================================================

float medirDistancia() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long duracao = pulseIn(PINO_ECHO, HIGH, 30000);

  if (duracao == 0) {
    return -1;
  }

  return duracao * 0.0343 / 2;
}

// =====================================================
// SAIDA DESCRITIVA
// =====================================================

void imprimirStatus(bool presenca, float distancia) {
  Serial.println("================================");
  Serial.println("       VIACIDADA IOT");
  Serial.println("================================");

  Serial.print("Dispositivo: ");
  Serial.println(ID_DISPOSITIVO);

  Serial.print("Parada: ");
  Serial.println(NOME_PARADA);

  Serial.print("Transporte: ");
  Serial.println(TIPO_TRANSPORTE);

  Serial.print("Presenca: ");
  Serial.println(presenca ? "PRESENTE" : "AUSENTE");

  Serial.print("Distancia: ");

  if (distancia < 0) {
    Serial.println("Sem leitura");
  } else {
    Serial.print(distancia, 2);
    Serial.println(" cm");
  }

  Serial.println("================================");
}

// =====================================================
// SAIDA JSON
// =====================================================

void imprimirJSON(bool presenca, float distancia) {
  Serial.println("JSON:");

  Serial.print("{");

  Serial.print("\"dispositivo\":\"");
  Serial.print(ID_DISPOSITIVO);

  Serial.print("\",\"parada\":\"");
  Serial.print(NOME_PARADA);

  Serial.print("\",\"transporte\":\"");
  Serial.print(TIPO_TRANSPORTE);

  Serial.print("\",\"presenca\":");
  Serial.print(presenca ? "true" : "false");

  Serial.print(",\"distancia_cm\":");

  if (distancia < 0) {
    Serial.print("null");
  } else {
    Serial.print(distancia, 2);
  }

  Serial.println("}");
  Serial.println();
}

// =====================================================
// PUBLICACAO NO MONITOR SERIAL
// =====================================================

void publicarStatus(bool presenca, float distancia) {
  imprimirStatus(presenca, distancia);
  imprimirJSON(presenca, distancia);
}

// =====================================================
// CONFIGURACAO INICIAL
// =====================================================

void setup() {
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  Serial.begin(9600);

  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERDE, LOW);

  Serial.println("ViaCidada IoT iniciado.");
}

// =====================================================
// MONITORAMENTO CONTINUO
// =====================================================

void loop() {
  float distancia = medirDistancia();

  bool presenca = (distancia >= 0 &&
                   distancia <= DISTANCIA_LIMITE);

  // CONTROLE DOS LEDS
  if (presenca) {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
  } else {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
  }

  unsigned long agora = millis();

  bool houveMudanca = presenca != estadoAnterior;

  bool passouUmMinuto =
      agora - ultimaAtualizacao >= INTERVALO_ATUALIZACAO;

  if (primeiraLeitura || houveMudanca || passouUmMinuto) {
    publicarStatus(presenca, distancia);

    estadoAnterior = presenca;
    ultimaAtualizacao = agora;
    primeiraLeitura = false;
  }

  delay(200);
}
