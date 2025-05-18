#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <WebServer.h>

// --- Configurações do Wi-Fi ---
const char* ssid = "";         
const char* password = "";    

// --- Configurações do DHT ---
#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//SUPABASE 
const char* supabase_url = "";
const char* supabase_key = "";

// --- Servidor HTTP na porta padrão (80) ---
WebServer server(80);

// --- Contador de leituras ---
int leituraCount = 0;

// --- Variáveis para leitura atual ---
float temperatura = 0.0;
float umidade = 0.0;

// --- Conecta à rede Wi-Fi ---
void conectaWiFi() {
  Serial.print("Conectando-se ao Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();  
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());
}

// --- Imprime cabeçalho da tabela no Serial ---
void imprimeCabecalho() {
  Serial.println("-------------------------------");
  Serial.println("| Temperatura (°C) | Umidade (%) |");
  Serial.println("-------------------------------");
}


// --- Atualiza Supabase com dados ---
void atualizaSupabase(float temperatura, float umidade) {
  HTTPClient http;
  String endpoint = String(supabase_url) + "/rest/v1/sensores";
  
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabase_key);
  http.addHeader("Authorization", "Bearer " + String(supabase_key));

  String payload = "{\"temperatura\": " + String(temperatura, 2) +
                   ", \"umidade\": " + String(umidade, 2) + "}";

  int code = http.POST(payload);
  String response = http.getString();

  //Serial.println(code);
  //Serial.println(response);

  http.end();
}

// --- Página HTML enviada via Wi-Fi ---
String gerarPaginaHTML(float temp, float hum) {
  String html = "<!DOCTYPE html><html lang='pt-br'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Relatório Climático de Ambiente</title>";
  html += "<meta name='theme-color' content='#444444'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; padding: 30px; margin: 0; }";
  html += "h1 { color: #222; margin-bottom: 20px; }";
  html += ".card { background: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.2); display: inline-block; }";
  html += "p { font-size: 22px; margin: 10px 0; }";
  html += "</style>";
  html += "<script>";
  html += "setTimeout(() => { location.reload(); }, 60000);";  // Recarrega a página a cada 60 segundos
  html += "</script>";
  html += "</head><body>";
  html += "<div class='card'>";
  html += "<h1>Relatório Climático de Ambiente</h1>";
  html += "<p>🌡️ Temperatura: " + String(temp, 1) + "°C</p>";
  html += "<p>💧 Umidade: " + String(hum, 1) + "%</p>";
  html += "<p><small>Atualizado a cada 5 segundos</small></p>";
  html += "</div>";
  html += "</body></html>";
  return html;
}

// --- Lida com acesso à raiz "/"
void handle_OnConnect() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  server.send(200, "text/html", gerarPaginaHTML(temp, hum));
}

// --- Página 404
void handle_NotFound() {
  server.send(404, "text/plain", "Página não encontrada");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  conectaWiFi();
  imprimeCabecalho();

  // Configura rotas
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  // Inicia Servidor Web
  server.begin();
  Serial.println("Servidor HTTP iniciado");

}

void loop() {
  // Atualiza leitura dos sensores
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  leituraCount++;

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na leitura do sensor DHT!");
  } else {
    Serial.print("|       ");
    Serial.print(temperatura, 1);
    Serial.print("       |     ");
    Serial.print(umidade, 1);
    Serial.println("     |");        
  }

  // A cada 10 leituras, imprime cabeçalho e IP
  if (leituraCount % 10 == 0) {
    Serial.println();
    Serial.println("-------------------------------");
    Serial.print("Status Wi-Fi: ");
    Serial.print(WiFi.SSID());
    Serial.print(" | IP: ");
    Serial.println(WiFi.localIP());
    imprimeCabecalho();
  }

  atualizaSupabase(temperatura, umidade);

  server.handleClient();
  delay(60000);
}