#include <WiFi.h>              // Biblioteca para conexão Wi-Fi
#include <PubSubClient.h>      // Biblioteca para comunicação MQTT

// ==================== Configurações padrão ====================
// Esses valores podem ser alterados se necessário
const char* default_SSID = "Wokwi-GUEST";         // Nome da rede Wi-Fi
const char* default_PASSWORD = "";                // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "20.151.77.156"; // Endereço IP do Broker MQTT
const int default_BROKER_PORT = 1883;             // Porta usada para comunicação MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp101/cmd";   // Tópico usado para receber comandos
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp101/attrs"; // Tópico usado para enviar estado do LED
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp101/attrs/l"; // Tópico usado para enviar luminosidade
const char* default_ID_MQTT = "fiware_101";       // ID do cliente MQTT
const int default_D4 = 2;                         // Pino do LED onboard do ESP32 (GPIO2)

// Prefixo usado para formar os tópicos de comando (on/off)
const char* topicPrefix = "lamp101";

// ==================== Variáveis editáveis (baseadas nos defaults) ====================
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

// Objetos responsáveis pela conexão Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Variável que guarda o estado atual do LED ('0' = desligado, '1' = ligado)
char EstadoSaida = '0';

// ==================== Funções de inicialização ====================

// Inicia a comunicação serial (para debug via Serial Monitor)
void initSerial() {
    Serial.begin(115200);
}

// Conecta à rede Wi-Fi, exibindo mensagens no Serial Monitor
void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi(); // Garante conexão
}

// Configura o cliente MQTT (servidor e função callback para mensagens recebidas)
void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

// ==================== Funções principais do Arduino ====================

// Setup: executa apenas 1 vez no início
void setup() {
    InitOutput();   // Pisca o LED para indicar inicialização
    initSerial();   // Inicia comunicação serial
    initWiFi();     // Conecta ao Wi-Fi
    initMQTT();     // Conecta ao broker MQTT
    delay(5000);
    MQTT.publish(TOPICO_PUBLISH_1, "s|on"); // Envia mensagem inicial ao broker
}

// Loop principal: executa repetidamente
void loop() {
    VerificaConexoesWiFIEMQTT(); // Mantém conexões ativas
    EnviaEstadoOutputMQTT();     // Publica estado do LED
    handleLuminosity();          // Lê e publica luminosidade
    MQTT.loop();                 // Processa mensagens MQTT recebidas
}

// ==================== Conexão Wi-Fi ====================

// Tenta reconectar ao Wi-Fi se não estiver conectado
void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return; // Já conectado → não faz nada

    WiFi.begin(SSID, PASSWORD); // Inicia conexão
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print("."); // Aguarda conexão (imprime progresso)
    }

    // Mostra dados da conexão no Serial Monitor
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    // Garante que o LED comece desligado
    digitalWrite(D4, LOW);
}

// ==================== Callback MQTT ====================
// Executada automaticamente quando chega uma mensagem MQTT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i]; // Constrói a mensagem recebida
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Formata as mensagens esperadas
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Liga o LED se a mensagem for "on"
    if (msg.equals(onTopic)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }

    // Desliga o LED se a mensagem for "off"
    if (msg.equals(offTopic)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

// ==================== Verificação das conexões ====================
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT(); // Reconecta ao broker se necessário
    reconectWiFi();      // Reconecta ao Wi-Fi se necessário
}

// ==================== Envio do estado do LED ====================
void EnviaEstadoOutputMQTT() {
    if (EstadoSaida == '1') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|on");
        Serial.println("- Led Ligado");
    }
    if (EstadoSaida == '0') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|off");
        Serial.println("- Led Desligado");
    }
    Serial.println("- Estado do LED onboard enviado ao broker!");
    delay(1000); // Evita flood de mensagens
}

// ==================== Inicialização do LED (efeito de piscar) ====================
void InitOutput() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    boolean toggle = false;

    // Pisca o LED 10 vezes para indicar inicialização
    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(200);
    }
}

// ==================== Reconexão ao broker MQTT ====================
void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        
        // Tenta conectar
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); // Escuta comandos
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

// ==================== Leitura de luminosidade ====================
// Lê o valor analógico de um potenciômetro/sensor no pino 34 e converte para %
void handleLuminosity() {
    const int potPin = 34; // Pino analógico usado
    int sensorValue = analogRead(potPin); // Lê valor bruto (0–4095)
    int luminosity = map(sensorValue, 0, 4095, 0, 100); // Converte para escala 0–100%
    
    // Monta e envia a mensagem
    String mensagem = String(luminosity);
    Serial.print("Valor da luminosidade: ");
    Serial.println(mensagem.c_str());
    MQTT.publish(TOPICO_PUBLISH_2, mensagem.c_str());
}
