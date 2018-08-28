const int pinoBotao = D2; //PINO DIGITAL UTILIZADO PELO PUSH BUTTON
const int pinoLed = D1; //PINO DIGITAL UTILIZADO PELO LED

int buttonState = LOW; //this variable tracks the state of the button, low if not pressed, high if pressed
int ledState = -1; //this variable tracks the state of the LED, negative if off, positive if on
 
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 400;    // the debounce time; increase if the output flickers

void setup() {
pinMode(pinoBotao, INPUT_PULLUP); //DEFINE O PINO COMO ENTRADA / "_PULLUP" É PARA ATIVAR O RESISTOR INTERNO
  //DO ARDUINO PARA GARANTIR QUE NÃO EXISTA FLUTUAÇÃO ENTRE 0 (LOW) E 1 (HIGH)
pinMode(pinoLed, OUTPUT); //DEFINE O PINO COMO SAÍDA
digitalWrite(pinoLed, LOW); //LED INICIA DESLIGADO
Serial.begin(115200);
//ConfigDisplay();
}
void loop(){
  buttonState = digitalRead(pinoBotao);
  Serial.println(digitalRead(D1));
  delay(500);

  if ( (millis() - lastDebounceTime) > debounceDelay) {
    if((digitalRead(buttonState) == HIGH)){ //SE A LEITURA DO PINO FOR IGUAL A LOW, FAZ
        digitalWrite(pinoLed, LOW); //APAGA O LED
        Serial.println(digitalRead(D1));
        delay(500);
    }
    else { //SENÃO, FAZ
        digitalWrite(pinoLed, HIGH); //ACENDE O LED
        Serial.println(digitalRead(D1));
        delay(500);
    }
    
  }
  //mostrar_estado_botao();
}
