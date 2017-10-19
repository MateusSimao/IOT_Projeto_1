void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

/**
 * Função para desligar ou ligar um led
 * var porta  = Receberá a porta do led que a função irá trablhar
 * var acao   = Receberá a ação de desligar ou ligar o led, por exemplo, HIGH para ligar ou LOW para desligar
 * var atraso = Receberá o delay, caso seja nessessario, caso contrario só executara a ação 
 * exemplo: ligaDesligaLed(13, HIGH, 1000) irá ligar o led e deixando com um delay de 1 segundo para a proxima ação
 */
void ligaDesligaLed(int porta, int acao , int atraso = 0) {
  digitalWrite(porta, acao);
  if (atraso != 0) {
    delay(atraso);
  }
}

void loop() {
  ligaDesligaLed(13,HIGH,1000);
  ligaDesligaLed(13,LOW,1000);
}
