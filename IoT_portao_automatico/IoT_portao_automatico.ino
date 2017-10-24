// ==========================================================================
// INICIO: ESCOPO DAS BIBLIOTECAS
#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <Servo.h>
// ==========================================================================
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x62};                                     // Definindo a variavel que recebe o mac address.
// Definindo cabelahos de funções
void callback(char *topic, byte *payload, unsigned int length);
void ligaDesligaLedDigital(int porta, int acao, int atraso = 0, bool inverso = false);
void verificaMesagem(char topic);
// Definindo as variaveis que recebem alguma classe
Servo motor;
EthernetClient ethClient;
PubSubClient client("m10.cloudmqtt.com", 16815, callback, ethClient);
// Definindo as portas que serão usadas
int ledAzul         = 3;
int ledVerde        = 4;
int ledVermelho     = 5;
int portaBotao      = 6;
int portaMotorServo = 8;
int portaBuzzer     = 7;
int ledPortao       = A1;
// Portas da shield de Ethernet: 2, 10, 11, 12, 13
int estadobotao = LOW;
int ultimoEstado = 0;
int pos;
// ==========================================================================
// Funçao que irá receber o retorno do servidor, e tratara as mensagens do topico 
void callback(char *topic, byte *payload, unsigned int length)
{
  char c = payload[0];
  Serial.println(c);
  Serial.println(topic);

  // Desligando os leds azul e vermelho, caso algum esteja ligado, e piscando o verde
  // quando recebe alguma informação do MQTT
  desligaLedsRGB(ledAzul, ledVermelho);
  for (int i = 0; i <= 2; i++) {
    ligaDesligaLedDigital(ledVerde, LOW, 100, true); // piscar o led
  }

  if (strcmp(topic, "Portao") == 0) {
    Serial.println("aqui2");
    Serial.println(c);
    if (c == '1') {
      abreFechaPortao(1);
    } else if(c == '0') {
      abreFechaPortao(0);
    }
  }
  //verificaMesagem(topic, c);
  //if (strcmp(topic, "Portao") == 0) {
  //  Serial.println("aqui2");
  //  abreFechaPortao();
  //}

  byte *p = (byte *)malloc(length);
  memcpy(p, payload, length);
  free(p);
}

void ligaDesligaLedDigital(int porta, int acao, int atraso = 0, bool inverso = false) // Função para desligar ou ligar um led em portas digitais, com delay e opção inversa.
{
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

void desligaLedsRGB(int param1, int param2) // Função para desligar duas cores do rgb
{
  digitalWrite(param1, LOW);
  digitalWrite(param2, LOW);
}

void verificaMesagem(char *topic, char *action) // Função que irá verificar a mensagem e topico digitado e tomas as devidas ações
{
  if (strcmp(topic, "Portao") == 0) {
    Serial.println("aqui2");
    Serial.println(action);
    if (action == 1) {
      abreFechaPortao(1);
    }
    //abreFechaPortao();
  }
}

void tocaBuzzer(int portaBuzzer, int frequencia, int atrazo, int toques = 1 )
{
  for(int i = 0; i < toques; i++) {
    tone(portaBuzzer,frequencia);   
    delay(atrazo);
    noTone(portaBuzzer);
    delay(atrazo);
  }
}

void abreFechaPortao(int acao) // Função que irá abrir ou fechar o botão, 1 para abrir 0 para fechar
{
    if (ultimoEstado == 0 && acao == 1) {
      ultimoEstado = 1;

      tocaBuzzer(portaBuzzer, 1500, 500, 3);
      for (pos = 0; pos <= 90; pos++){
        motor.write(pos);
        delay(15);
      }
      delay(1000);
      digitalWrite(ledPortao, HIGH);
    } else if(ultimoEstado == 1 && acao == 0) {
      ultimoEstado = 0;
      tocaBuzzer(portaBuzzer, 1500, 500, 3);
      for (pos = 90; pos >= -10; pos--){
        motor.write(pos);
        delay(15);
      }
      delay(1000);
      digitalWrite(ledPortao, LOW);
    }
}
// ==========================================================================
void setup()
{
  // setup do motor servo
  motor.attach(portaMotorServo);
  motor.write(0);

  // setup botao
  pinMode(portaBotao, INPUT);
  pinMode(ledPortao, OUTPUT);

  // setup buzzer
  pinMode(portaBuzzer, OUTPUT);

  // setup do serial
  Serial.begin(9600);

  // iniciando a conexão
  Serial.println("Conectando...");
  ligaDesligaLedDigital(ledAzul, HIGH);
  Ethernet.begin(mac);

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (client.connect("arduino", "test", "test"))
  {
    ligaDesligaLedDigital(ledAzul, LOW);
    ligaDesligaLedDigital(ledVerde, HIGH);
    
    // Envia uma mensagem para o cloud no topic portao
    //client.publish("Portao", 1);
    //client.publish("LuzGaragem", 1);

    // Conectando nos topics para receber as mensagens
    client.subscribe("Portao");
    //client.subscribe("LuzGaragem");
    
    Serial.println("conectado!!");
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
  if (!client.connected()) {
    desligaLedsRGB(ledAzul, ledVerde);
    ligaDesligaLedDigital(ledVermelho, HIGH);
  }
  Serial.println(client.connected());
  if (digitalRead(portaBotao) == HIGH) {
    Serial.println("Botao clicado");
    if (ultimoEstado == 0) {
      abreFechaPortao(1);
    } else {
      abreFechaPortao(0);
    }
  }
}
