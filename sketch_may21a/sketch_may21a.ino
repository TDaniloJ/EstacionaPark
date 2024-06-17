#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char* ssid = "NOME DA REDE";       // Substitua pelo seu SSID
const char* password = "SENHA DA REDE";  // Substitua pela sua senha

const int ledPin1 = 13; // Pino do LED da vaga 1
const int echoPin1 = 14; // Pino do sensor (echo Saída) da vaga 1
const int trigPin1 = 26; // Pino do sensor (trig Entrada) da vaga 1

const int ledPin3 = 12; // Pino do LED da vaga 3
const int echoPin3 = 25; // Pino do sensor (echo Saída) da vaga 3
const int trigPin3 = 27; // Pino do sensor (trig Entrada) da vaga 3

bool vagaOcupada1 = false;
bool vagaOcupada3 = false;

long distancia1, duracao1;
long distancia3, duracao3;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT); // Configura o pino do LED como saída 
  pinMode(trigPin1, OUTPUT); // Configura o pino do sensor como saída
  pinMode(echoPin1, INPUT); // Configura o pino do sensor como entrada

  pinMode(ledPin3, OUTPUT); // Configura o pino do LED como saída 
  pinMode(trigPin3, OUTPUT); // Configura o pino do sensor como saída
  pinMode(echoPin3, INPUT); // Configura o pino do sensor como entrada

  // Conecta ao WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Configura o servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String status1 = vagaOcupada1 ? "Ocupada" : "Livre";
    String status3 = vagaOcupada3 ? "Ocupada" : "Livre";
    String html = "<!DOCTYPE html><html lang='pt-BR'><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<meta http-equiv='refresh' content='5'>"; // Adiciona recarregamento automático a cada 5 segundos
    html += "<title>Estaciona Park - Status das Vagas</title><style>";
    html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background: #f4f4f9; }";
    html += "header { position: fixed; top: 0; width: 100%; background: #333; color: white; padding: 10px 0; text-align: center; z-index: 1000; }";
    html += "header nav ul { list-style-type: none; padding: 0; margin: 0; }";
    html += "header nav ul li { display: inline; margin: 0 15px; }";
    html += "header nav ul li a { color: white; text-decoration: none; font-weight: bold; }";
    html += ".container { padding: 80px 20px 20px; max-width: 800px; margin: auto; }";
    html += ".vagas { display: flex; flex-wrap: wrap; gap: 20px; }";
    html += ".vaga { flex: 1 1 calc(33.333% - 20px); padding: 10px; margin: 10px 0; border-radius: 8px; background: #e0e0e0; }";
    html += "h1 { color: #333; }";
    html += "p { font-size: 1.2em; }";
    html += ".status { padding: 20px; border-radius: 8px; background: ";
    html += "footer { position: fixed; bottom: 0; width: 100%; background: #333; color: white; text-align: center; padding: 10px 0; }";
    html += "footer p { margin: 0; }";
    html += "section { margin-bottom: 40px; }";
    html += "section h2 { color: #555; margin-bottom: 10px; }";
    html += "section p { color: #777; }";
    html += "</style></head><body>";
    html += "<header><nav><ul><li><a href='/'>Home</a></li><li><a href='/about'>About</a></li><li><a href='/contact'>Contact</a></li></ul></nav></header>";
    html += "<div class='container'>";
    html += "<h1>Estaciona Park</h1>";  // Adiciona o título aqui
    html += "<section><h1>Status das Vagas</h1>";
    html += "<div class='vagas'>";
    html += "<div class='vaga'><h2>Vaga 1</h2><p class='status' style='background:" + String(vagaOcupada1 ? "#ff4d4d; color: white;" : "#4caf50; color: white;") + "'>Vaga 1 está " + status1 + "</p></div>";
    html += "<div class='vaga'><h2>Vaga 2</h2><p class='status' style='background:#ff4d4d; color: white;'>Vaga 2 está Indisponível</p></div>";
    html += "<div class='vaga'><h2>Vaga 3</h2><p class='status' style='background:" + String(vagaOcupada3 ? "#ff4d4d; color: white;" : "#4caf50; color: white;") + "'>Vaga 3 está " + status3 + "</p></div>";
    html += "<div class='vaga'><h2>Vaga 4</h2><p class='status' style='background:#ff4d4d; color: white;'>Vaga 4 está Indisponível</p></div>";
    html += "<div class='vaga'><h2>Vaga 5</h2><p class='status' style='background:#ff4d4d; color: white;'>Vaga 5 está Indisponível</p></div>";
    html += "<div class='vaga'><h2>Vaga 6</h2><p class='status' style='background:#ff4d4d; color: white;'>Vaga 6 está Indisponível</p></div>";
    html += "<div class='vaga'><h2>Vaga 7</h2><p class='status' style='background:#ff4d4d; color: white;'>Vaga 7 está Indisponível</p></div>";
    html += "</div></section>";
    html += "<section><h2>Sobre o Sistema</h2><p>Este sistema monitora as vagas de estacionamento utilizando sensores ultrassônicos e exibe o status em tempo real através de uma página web.</p></section>";
    html += "<section><h2>Contato</h2><p>Para mais informações, entre em contato conosco pelo e-mail: contato@exemplo.com</p></section>";
    html += "</div>";
    html += "<footer><p>&copy; 2024 Sistema de Monitoramento de Vagas. Todos os direitos reservados.</p></footer>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // Desliga os LEDs no início do loop
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin3, LOW);

  // Gera um pulso ultrassônico para vaga 1
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  // Calcula a duração do pulso para vaga 1
  duracao1 = pulseIn(echoPin1, HIGH);
  distancia1 = (duracao1 / 2) * 0.0343; // Converte a duração em distância

  // Log de depuração para vaga 1
  Serial.print("Vaga 1 - Duração do pulso: ");
  Serial.print(duracao1);
  Serial.print(" us, Distância: ");
  Serial.print(distancia1);
  Serial.println(" cm");

  // Verifica se a distância é válida para vaga 1
  if (distancia1 <= 0) {
    Serial.println("Distância inválida para Vaga 1.");
  }

  // Define o estado da vaga 1 e do LED
  if (distancia1 > 0 && distancia1 < 8) { // Se a distância for válida e menor que 8 cm
    vagaOcupada1 = true;
    digitalWrite(ledPin1, HIGH); // Acende o LED da vaga 1
  } else {
    vagaOcupada1 = false;
  }

  // Gera um pulso ultrassônico para vaga 3
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);

  // Calcula a duração do pulso para vaga 3
  duracao3 = pulseIn(echoPin3, HIGH);
  distancia3 = (duracao3 / 2) * 0.0343; // Converte a duração em distância

  // Log de depuração para vaga 3
  Serial.print("Vaga 3 - Duração do pulso: ");
  Serial.print(duracao3);
  Serial.print(" us, Distância: ");
  Serial.print(distancia3);
  Serial.println(" cm");

  // Verifica se a distância é válida para vaga 3
  if (distancia3 <= 0) {
    Serial.println("Distância inválida para Vaga 3.");
  }

  // Define o estado da vaga 3 e do LED
  if (distancia3 > 0 && distancia3 < 8) { // Se a distância for válida e menor que 8 cm
    vagaOcupada3 = true;
    digitalWrite(ledPin3, HIGH); // Acende o LED da vaga 3
  } else {
    vagaOcupada3 = false;
  }

  delay(100);  // Pequeno delay para estabilidade do loop

  // Limpa o buffer Serial
  Serial.flush();
}
