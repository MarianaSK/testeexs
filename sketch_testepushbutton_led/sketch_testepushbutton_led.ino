/*#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);*/

const int pinoBotao = D2; //PINO DIGITAL UTILIZADO PELO PUSH BUTTON
const int pinoLed = D1; //PINO DIGITAL UTILIZADO PELO LED

void setup() {
pinMode(pinoBotao, INPUT_PULLUP); //DEFINE O PINO COMO ENTRADA / "_PULLUP" É PARA ATIVAR O RESISTOR INTERNO
  //DO ARDUINO PARA GARANTIR QUE NÃO EXISTA FLUTUAÇÃO ENTRE 0 (LOW) E 1 (HIGH)
pinMode(pinoLed, OUTPUT); //DEFINE O PINO COMO SAÍDA
digitalWrite(pinoLed, LOW); //LED INICIA DESLIGADO
Serial.begin(115200);
//ConfigDisplay();
}
void loop(){
  if(digitalRead(pinoBotao) == LOW){ //SE A LEITURA DO PINO FOR IGUAL A LOW, FAZ
      digitalWrite(pinoLed, LOW); //APAGA O LED
      Serial.println(digitalRead(D2));
      delay(500);
  }else{ //SENÃO, FAZ
      digitalWrite(pinoLed, HIGH); //ACENDE O LED
      Serial.println(digitalRead(D2));
      delay(500);
  }
  //mostrar_estado_botao();
}
/*void ConfigDisplay(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();  
}
void mostrar_estado_botao(){
  if(digitalRead(pinoBotao) == LOW)
    exibe_display("low ", (pinoBotao));
  //exibe_display("Umidade ",(umidade)," %");
  else
    exibe_display("high ", (pinoBotao));
  
}

//EXIBE TEMPERATURA E UMIDADE NO DISPLAY
void exibe_display(const char* texto1, bool pinoBotao){
  display.clearDisplay(); //limpa o display
  display.setTextSize(1); //1a linha
  display.setCursor(0,0);
  display.print(texto1);
  display.setTextSize(3); // 2a linha
  display.setCursor(0,20);
  display.print(pinoBotao);
  //display.setTextSize(2);
  //display.print(texto2);
  display.display();
  delay(2000);
}*/



