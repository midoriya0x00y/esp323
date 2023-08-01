ESSE CODIGO ESTA SEM A TELA !!!

#include <WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>

// Substitua pelas suas informações de rede
const char* ssid = "SUA_REDE_SSID";
const char* password = "SUA_REDE_PASSWORD";

// Substitua pelo seu token de escrita do ThingSpeak
const char* apiKey = "3XDC41J0OA678Q1X";

// Configuração do sensor DHT22
#define DHTPIN 15
#define DHTTYPE DHTesp::DHT22
DHTesp dht;

WiFiClient espClient;
PubSubClient client(espClient);

// Substitua pelas informações do broker MQTT
const char* mqttBroker = "test.mosquitto.org";
const int mqttPort = 1883;

// Tópico MQTT para publicar as leituras
const char* mqttTopic = "esp32/temperatura";

void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void conectarMQTT() {
  Serial.print("Conectando ao broker MQTT...");
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falha na conexão ao broker MQTT, erro: ");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
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

  client.setServer(mqttBroker, mqttPort);
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

    // Publica as leituras no tópico MQTT
    if (client.connected()) {
      char msg[50];
      snprintf(msg, 50, "{\"temperatura\": %.2f, \"umidade\": %.2f}", temperatura, umidade);
      client.publish(mqttTopic, msg);
    }

    // Envia os dados para o ThingSpeak
    enviarParaThingSpeak(temperatura, umidade);
  } else {
    Serial.println("Falha na leitura do sensor DHT22");
  }

  // Mantém a conexão com o broker MQTT
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  delay(5000); // Intervalo entre as leituras
}
