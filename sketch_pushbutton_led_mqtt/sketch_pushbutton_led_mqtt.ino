// Libs
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Vars
const char* SSID = "testeiot"; // rede wifi
const char* PASSWORD = "mkuchida"; // senha da rede wifi

const char* BROKER_MQTT = "iot.eclipse.org"; // ip/host do broker
int BROKER_PORT = 1883; // porta do broker

const int ledPin = D1; //led attached to this pin
const int buttonPin = D2; //push button attached to this pin
 
int buttonState = LOW; //this variable tracks the state of the button, low if not pressed, high if pressed
int ledState = -1; //this variable tracks the state of the LED, negative if off, positive if on
 
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time; increase if the output flickers
 
// prototypes
void initPins();
void initSerial();
void initWiFi();
void initMQTT();

WiFiClient espClient;
PubSubClient MQTT(espClient); // instancia o mqtt

// setup
void setup() {
  initPins();
  initSerial();
  initWiFi();
  initMQTT();
}

void loop() {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }
  recconectWiFi();
  MQTT.loop();
  buttonState = digitalRead(buttonPin);
 Serial.println(digitalRead(D1));
      delay(500);
  //filter out any noise by setting a time buffer
  if ( (millis() - lastDebounceTime) > debounceDelay) {
 
    //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
    if ( (buttonState == HIGH) && (ledState < 0) ) {
 
      digitalWrite(ledPin, HIGH); //turn LED on
      ledState = -ledState; //now the LED is on, we need to change the state
      lastDebounceTime = millis(); //set the current time
      /*Serial.println(digitalRead(D1));
      delay(500);*/
    }
    else if ( (buttonState == HIGH) && (ledState > 0) ) {
 
      digitalWrite(ledPin, LOW); //turn LED off
      ledState = -ledState; //now the LED is off, we need to change the state
      lastDebounceTime = millis(); //set the current time
      /*Serial.println(digitalRead(D1));
      delay(500);*/
    }//close if/else
 
  }//close if(time buffer)
  
}

// implementacao dos prototypes

void initPins() {
  pinMode(buttonPin, INPUT);
  pinMode(D1, OUTPUT);//inicialmente D5
  digitalWrite(D1, 0);
}

void initSerial() {
  Serial.begin(115200);
}
void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}

// Funcão para se conectar ao Broker MQTT
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

//Função que recebe as mensagens publicadas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  String message;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));
  if (message == "1") {
    digitalWrite(D1, 1);
  } else {
    digitalWrite(D1, 0);
  }
  Serial.flush();
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Tentando se conectar ao Broker MQTT: " + String(BROKER_MQTT));
    if (MQTT.connect("ESP8266-ESP12-E")) {
      Serial.println("Conectado");
      MQTT.subscribe("mariiot/LED");

    } else {
      Serial.println("Falha ao Reconectar");
      Serial.println("Tentando se reconectar em 2 segundos");
      delay(2000);
    }
  }
}

void recconectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
}


