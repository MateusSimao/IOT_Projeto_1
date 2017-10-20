// ==========================================================================
// INICIO: ESCOPO DAS BIBLIOTECAS
#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <Servo.h>
// ==========================================================================
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x62};                                     // Definindo a variavel que recebe o mac address.
void callback(char *topic, byte *payload, unsigned int length);                        // Definindo o cabeçalho da função callback
void ligaDesligaLedDigital(int porta, int acao, int atraso = 0, bool inverso = false);

Servo s;                                                              // Variável Servo
EthernetClient ethClient;                                             // Definindo a variavel que recebe o cliente internet
PubSubClient client("m10.cloudmqtt.com", 16815, callback, ethClient); // Dados do MQTT Cloud

int portaMotorServo = 6;
int ledAzul         = 5; // Definindo a porta do led rgb azul
int ledVermelho     = 3; // Definindo a porta do led rgb vermelho
int ledVerde        = 4; // Definindo a porta do led rgb verde

int pos;
// ==========================================================================
// Funçao que irá receber o retorno do servidor, e tratara as mensagens do topico 
void callback(char *topic, byte *payload, unsigned int length)
{
  char c = payload;
  Serial.println(c);
  Serial.println(topic);

  // Desligando os leds azul e vermelho, caso algum esteja ligado, e piscando o verde
  // quando recebe alguma informação do MQTT
  desligaLedsRGB(ledAzul, ledVermelho);
  ligaDesligaLedDigital(ledVerde, LOW, 200, true); // piscar o led
  
  //verificaMesagem(topic);

  byte *p = (byte *)malloc(length);
  memcpy(p, payload, length);
  free(p);
}

// Função para desligar ou ligar um led em portas digitais, com delay e opção inversa.
void ligaDesligaLedDigital(int porta, int acao, int atraso = 0, bool inverso = false) {
  pinMode(porta, OUTPUT);
  digitalWrite(porta, acao);
  if (atraso != 0) {
    delay(atraso);
  }
  if (inverso == true && atraso != 0) {
    digitalWrite(porta, !digitalRead(porta));
    delay(atraso);
  }
}

// Função para desligar duas cores do rgb
void desligaLedsRGB(int param1, int param2) {
  digitalWrite(param1, LOW);
  digitalWrite(param2, LOW);
}

// Função que irá verificar a mensagem e topico digitado e tomas as devidas ações
void verificaMesagem(char topic) {
  if (strcmp("Mensagem", topic) == 0) {
    
  }
}
// ==========================================================================
void setup()
{
  // setup do motor servo
  s.attach(portaMotorServo);
  s.write(30);

  // setup do serial
  Serial.begin(9600);

  // iniciando a conexão
  Serial.println("Iniciando...");
  ligaDesligaLedDigital(ledAzul, HIGH);
  Ethernet.begin(mac);

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (client.connect("arduino", "test", "test"))
  {
    ligaDesligaLedDigital(ledAzul, LOW);
    ligaDesligaLedDigital(ledVerde, HIGH);
    
    // Envia uma mensagem para o cloud no topic portao
    client.publish("mensagem", 1);

    // Conectando nos topics para receber as mensagens
    client.subscribe("mensagem");
    
    Serial.println("conectado Temperatura");
  }else{
    desligaLedsRGB(ledAzul, ledVerde);
    ligaDesligaLedDigital(ledVermelho, HIGH);
    Serial.println("erro ao conectar");  
  }
  
}
// ==========================================================================
void loop()
{
  client.loop(); 
}