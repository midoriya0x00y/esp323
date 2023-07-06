#include <WiFi.h>
#include <DHTesp.h>
#include <HTTPClient.h>

// Substitua pelas suas informações de rede
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Substitua pelo seu token de escrita do ThingSpeak
const char* apiKey = "3XDC41J0OA678Q1X";

// Configuração do sensor DHT22
#define DHTPIN 15
#define DHTTYPE DHTesp::DHT22
DHTesp dht;

WiFiClient client;

void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void enviarParaThingSpeak(float temperatura, float umidade) {
  String url = "http://api.thingspeak.com/update?api_key=";
  url += apiKey;
  url += "&field1=";
  url += String(temperatura);
  url += "&field2=";
  url += String(umidade);

  // Realiza a solicitação HTTP GET
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Dados enviados para o ThingSpeak");
  } else {
    Serial.println("Falha ao enviar os dados para o ThingSpeak");
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  dht.setup(DHTPIN, DHTTYPE);
}

void loop() {
  // Lê a temperatura e a umidade do sensor DHT22
  float temperatura = dht.getTemperature();
  float umidade = dht.getHumidity();

  // Verifica se a leitura foi bem-sucedida
  if (!isnan(temperatura) && !isnan(umidade)) {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C, Umidade: ");
    Serial.print(umidade);
    Serial.println("%");

    // Envia os dados para o ThingSpeak
    enviarParaThingSpeak(temperatura, umidade);
  } else {
    Serial.println("Falha na leitura do sensor DHT22");
  }

  delay(5000); // Intervalo entre as leituras
}
