# 🌐 ESP32 IoT com FIWARE e MQTT

Este projeto utiliza um **ESP32** conectado a um **broker MQTT** em nuvem para controlar um LED onboard e monitorar luminosidade.  
A integração foi feita com o **FIWARE** e testada via **Postman**, conforme orientado pelo professor **Fábio Cabrini** em seu [repositório FIWARE Descomplicado](https://github.com/fabiocabrini/fiware).  

O professor também explica como configurar uma máquina virtual para suportar o **FIWARE** e o **MQTT** na nuvem, tornando o projeto aplicável em cenários reais de IoT.

---

## Componentes

- **ESP32**
- **LED onboard** (GPIO2)
- **Potenciômetro ou LDR** (conectado ao pino `GPIO34`)
- **Cabos jumper**
- **Protoboard** (opcional, para montagem organizada)

---

## Diagrama de Ligação

| Componente | Pino ESP32 |
|------------|------------|
| LED Onboard | GPIO2 (já incluso no ESP32) |
| Potenciômetro/LDR | GPIO34 (entrada analógica) |

---

## Instalação de Software

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Instale as bibliotecas necessárias pelo Gerenciador de Bibliotecas:
   - `WiFi.h` (já incluída por padrão)
   - `PubSubClient`
3. Configure o código com suas credenciais de rede e broker MQTT:
   ```cpp
   const char* default_SSID = "Wokwi-GUEST"; // Nome da rede
   const char* default_PASSWORD = ""; // senha da rede
   const char* default_BROKER_MQTT = "20.163.23.245"; // Endereço de ip da VM
   const int default_BROKER_PORT = 1883;


**Lembre-se também de alterar o ID da lampada no código e no postman se for fazer o teste do projeto através Wokwi 
   ```cpp
   const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp101/cmd";   
   const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp101/attrs"; 
   const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp101/attrs/l"; 
   const char* default_ID_MQTT = "fiware_101";       
   const char* topicPrefix = "lamp101";
   ```

## Simulação do Código
Simule esse projeto em https://wokwi.com/projects/441017555603997697

## Mais informações 
Para mais informações e explicações de como utilizar a ferramenta FIWARE e outras como MongoDB, Postman visite o repositório do professor Fabio Cabrini [FIWARE Descomplicado](https://github.com/fabiocabrini/fiware)

## Colaboradores

- Cesar Aaron Herrera
- Kaue Soares Madarazzo
- Rafael Seiji Aoke Arakaki
- Rafael Yuji Nakaya
- Nicolas Mendes dos Santos

## Agradecimentos

- Professor Fabio Cabrini (Disciplina: Edge Computing and Computer Systems, FIAP)
  

> **Observação:** Este projeto foi desenvolvido como parte da disciplina *Edge Computing and Computer Systems* na FIAP.
