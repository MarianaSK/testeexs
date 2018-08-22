/*// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "testeiot";
const char* password = "mkuchida";
WiFiClient nodemcuClient;

/* --- MQTT ---
/*#include <PubSubClient.h>
const char* mqtt_broker = "iot.eclipse.org";
const char* mqtt_clientID = "termometro-escritorio";
PubSubClient client(nodemcuClient); //objeto associado a uma conexão wifi
const char* topicoTemperatura = "mariiotteste/temperatura";
const char* topicoUmidade = "mariiotteste/umidade";*/

/*// --- DHT ---
#include <DHT.h>

#define DHTPIN D3
#define DHTTYPE DHT22 //selecionar o tipo de sensor 

DHT dht(DHTPIN,DHTTYPE);

float umidade;
float temperatura;
//float setpoint = 22;
*/
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"
//--- DISPLAY ---
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
#include <OLEDDisplay.h>
#include <U8glib.h>
Adafruit_SSD1306 display(OLED_RESET);
#include "img.h"
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
//SSD1306Wire  display(0x3c, D3, D5);

//--- SETUP ---
void setup() {
  Serial.begin(115200); 
  ConfigDisplay();
 // conectarWifi();
 //client.setServer(mqtt_broker, 1883);

}

// --- INICIALIZAÇÃO ---
/*void drawFontFaceDemo() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Bem vindo");
}*/
// --- LOOP ---
void loop() {
  /*if (!client.connected()){
    reconectarMQTT(); // se o cliente não estiver conectado, busca a função de reconectar
  }*/
  exibe_display();
  //medirTemperaturaUmidade(); //1o mede
  //publicarTemperaturaUmidadeNoTopico(); //2o publica
  //mostrar_temp_umid(); // depois mostra
}

// --- FUNÇÕES AUXILIARES ---

// --- CONFIGURAÇÃO E CONFIGURAÇÃO WIFI ---
/*void conectarWifi(){
  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }

}

// --- RECONECTAR O CLIENTE MQTT ---
void reconectarMQTT(){
     while (!client.connected()) {
      client.connect(mqtt_clientID);// enquanto não estiver conectado ele chama a função connect
      // e deve se referir a alguém
     }
}*/

//--- CONFIGURA O DISPLAY ---
void ConfigDisplay(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();  
}

/*// --- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaUmidadeNoTopico(){
   client.publish(topicoTemperatura, String(temperatura).c_str(), true); // true = permite que o cliente possa visualizar o ultimo dado no broker sem ficar esperando a proxima atualização
   client.publish(topicoUmidade, String(umidade).c_str(), true);
}*/

/*//--- MEDIÇÃO DE TEMPERATURA E UMIDADE ---
void medirTemperaturaUmidade() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("*C");
  Serial.print("    ");
  Serial.print("Umidade: ");
  Serial.println(umidade);
  delay(5000);
}

//MOSTRA TEMPERATURA E UMIDADE
void mostrar_temp_umid(){
//  exibe_display("Set Point:", (setpoint), " C");
  exibe_display("Temperatura ", (temperatura)," C","Umidade ",(umidade)," %");
  //exibe_display("Umidade ",(umidade)," %");
}*/

//EXIBE TEMPERATURA E UMIDADE NO DISPLAY
void exibe_display(){
  display.clearDisplay(); //limpa o display
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperatura:");
  display.setTextSize(2); // 2a linha
  display.setCursor(0,10);
  display.print("21.10 C");
  display.setTextSize(1);
  display.setCursor(0,30);
  display.print("Umidade: 51.40 %");
  display.drawCircle(40,45,5,WHITE);
  display.fillCircle(100,45,5,WHITE);
  //display.drawXbm(40,70,50,50,img.h);
  display.setTextSize(1);
  display.setCursor(40,55);
  display.print("off");
  display.setTextSize(1);
  display.setCursor(100,55);
  display.print("on");
  display.display();
  delay(250);  
}


