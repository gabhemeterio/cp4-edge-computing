//Integrantes: Felippe Nascimento Silva | Gabriel dos Santos Hemeterio | Matheus Hideki Doroszewski Yoshimura 
//RMs: 562123 | 566243 | 564970

#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// === CONFIGURAÇÃO DHT ===
#define DHTPIN 4       // GPIO4
#define DHTTYPE DHT22  // Modelo DHT22

DHT dht(DHTPIN, DHTTYPE);

// === CONFIGURAÇÕES DE REDE E THINGSPEAK ===
const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";
String apiKey = "0SPWED7TMFO6L58K";
const char* server = "http://api.thingspeak.com";

// === PINO LDR ===
#define LDR_PIN 34

void setup() {
  Serial.begin(115200);

  // Inicializa DHT22
  dht.begin();
  delay(2000); // aguarda estabilização do sensor

  // Conexão Wi-Fi
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void loop() {
  // Leitura do DHT22
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Verifica se leitura é válida
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na leitura DHT22. Tentando novamente...");
    delay(2000);
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
  }

  // Leitura LDR
  int valorLDR = analogRead(LDR_PIN);
  int luminosidade = map(valorLDR, 0, 4095, 100, 0);

  // Debug Serial
  Serial.println("========== LEITURA ==========");
  Serial.println("Temperatura: " + String(temperatura,1) + " °C");
  Serial.println("Umidade: " + String(umidade,1) + " %");
  Serial.println("Luminosidade: " + String(luminosidade) + " %");
  Serial.println("=============================");

  // Envio para ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "/update?api_key=" + apiKey;
    url += "&field1=" + String(temperatura);
    url += "&field2=" + String(umidade);
    url += "&field3=" + String(luminosidade);

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Enviado ao ThingSpeak. Código HTTP: " + String(httpCode));
    } else {
      Serial.println("Erro ao enviar: " + String(http.errorToString(httpCode).c_str()));
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado!");
  }

  delay(16000); // intervalo recomendado pelo ThingSpeak
}
