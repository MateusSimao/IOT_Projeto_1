// ==========================================================================
// INICIO: ESCOPO DAS BIBLIOTECAS
#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <Servo.h>


// ==========================================================================
// INICIO: ESCOPO DAS VARIAVEIS
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x62};                   // Definindo a variavel que recebe o mac address.
void callback(char *topic, byte *payload, unsigned int length);      // Definindo a variavel que recebe o cabeçalho da função callback
int pos;                                                             // Posição Servo
Servo s;                                                             // Variável Servo
EthernetClient ethClient;                                            // Definindo a variavel que recebe o cliente internet
PubSubClient client("m10.cloudmqtt.com", 16815, callback, ethClient);// Dados do MQTT Cloud
int ledAzul     = 5;
int ledVermelho = 3;
int ledVerde    = 4;

// ==========================================================================
// INICIO: ESCOPO DAS FUNCOES
/** 
 * Funcçao que irá receber o retorno do servidor. 
 */
void callback(char *topic, byte *payload, unsigned int length)
{
  char c = payload;
  Serial.println(c);
  Serial.println(topic);

  if(strcmp("mensagem", topic) == 0){
    for(pos = 0; pos < 90; pos++)
    {
      s.write(pos);
      delay(15);
    }
    delay(1000);
    for(pos = 90; pos >= 0; pos--)
    {
      s.write(pos);
      delay(15);
    }
  }

  byte *p = (byte *)malloc(length);
  memcpy(p, payload, length);
  free(p);
}

/**
 * Descrição     = Função para desligar ou ligar um led em portas digitais.
 * param porta   = Receberá a porta do led que a função irá trablhar.
 * param acao    = Receberá a ação de desligar ou ligar o led, por exemplo, HIGH para ligar ou LOW para desligar.
 * param atraso  = Receberá o delay, caso seja nessessario, caso contrario só executara a ação.
 * param inverso = Receberá true caso queira que ele faça o inverso da primeira acao,delay obrigatorio.
 * exemplo       = ligaDesligaLed(13, HIGH, 1000) irá ligar o led e deixando com um delay de 1 segundo para a proxima ação
 */
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

void setup()
{
  //Servo
  s.attach(6);
  s.write(0);
  
  Serial.begin(9600);
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

    // Conecta no topic para receber mensagens
    client.subscribe("mensagem");
    
    Serial.println("conectado Temperatura");
  }else{
    ligaDesligaLedDigital(ledAzul, LOW);
    ligaDesligaLedDigital(ledVerde, LOW);
    ligaDesligaLedDigital(ledVermelho, HIGH);
    Serial.println("erro ao conectar");  
  }
  
}

void loop()
{
  client.loop();
  
}
