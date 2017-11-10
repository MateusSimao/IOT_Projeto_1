#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x62 }; // Definindo a variavel que recebe o mac address.

void callback(char* topic, byte* payload, unsigned int length) ;

EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 12719, callback, ethClient);
boolean reconnect() // Recebe função que reconecta ao servidor MQTT
{
  Serial.println("reconectando...");
  if (client.connect("arduinoClient", "usuario", "senha")) {
    client.subscribe("vaga/22");
  }
  return client.connected();
}
const int rs = 9, en = 8, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int vagas_disponiveis = 0;
int estadoVagas = 0;
long lastReconnectAttempt = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  int c = atoi(payload);
  Serial.println(topic);
  Serial.println(c);
  if (c == 0) {
    vagas_disponiveis--;
  }
  if (c == 1) {
    vagas_disponiveis++;
  }
}

void setup()
{
  // Setup do Serial
  Serial.begin(9600);
  
  // Setup do Ethernet
  Ethernet.begin(mac);

  // Setup do LCD
  lcd.begin(16, 2);
}

void loop()
{
  if (!client.connected()) { // Verificando se esta conectado
    long now = millis(); // Pegando o milisegundo atual da aplicação

    if (now - lastReconnectAttempt > 5000) { // Verificando se já passou 5 segundos desde a ultima tentativa
      Serial.println("reconectando...");
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("RECONECTANDO");

      lastReconnectAttempt = now; // Atribuindo o milisegundo atual para a futura comparação(se necessario)

      if (reconnect()) { // Executando a função da variavel reconnect, e verificando se retornou true
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("CONECTADO");

        delay(1000);

        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
    if (estadoVagas != vagas_disponiveis) {
      estadoVagas = vagas_disponiveis;
      
      lcd.setCursor(0, 1);
      lcd.print("              ");
      
      lcd.setCursor(0, 1);
      lcd.print(vagas_disponiveis);
    }
  }
}