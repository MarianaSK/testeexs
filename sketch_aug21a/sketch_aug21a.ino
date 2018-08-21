#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN D3
#define DHTTYPE DHT22 //selecionar o tipo de sensor 
#define ONE_WIRE_BUS D1  // DS18B20 pin

DHT dht(DHTPIN,DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

//float umidade;
float temperatura;
float temp;

void setup() {
  Serial.begin(115200); 
  DS18B20.begin(); 
}

void loop() {
  tempdht ();
  tempds();
    /*Serial.print("Umidade: ");
  Serial.println(umidade);*/
}

// --- TEMPERATURA DHT22 ---
void tempdht (){
  //umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("*C");
  Serial.println("    ");
  delay(5000);
}
// --- TEMPERATURA DO DS18B20 ---
void tempds(){
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(false);
  Serial.print("Temperature: ");
  Serial.print(temp, true);
  Serial.print("*C");
  delay(6000);
}

// --- DS18B20 ---
//#include <esp8266wifi.h>
//#include <esp8266httpclient.h>

 
/*const char* ssid = "<ssid to="" connect="">";
const char* password = "<password>";
 
WiFiServer server(80);
 
String url = "";
bool httpFunction = false;*/
 


//void setup() {
  //Serial.begin(115200);
  //delay(5000);
  // Connect to WiFi network
  /*Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}*/
 
//void loop() {
 
 
    /*url="http://<server ip="">/t?temp="+String(temp);
    httpConnect(url);
    delay(1000);*/
//}
 
/*Função httpConnect()
bool httpConnect(String url) {
    HTTPClient http;  // Declare an object of class HTTPClient
    http.begin(url);  // Specify request destination
    int httpCode = http.GET(); //Send the request
    if (httpCode > 0) { // Check the returning code
      String payload = http.getString();   //Get the request response payload
    }
    http.end();   // Close connection
    return true;
}
</server></password></ssid></dallastemperature.h></onewire.h></esp8266httpclient.h></esp8266wifi.h*/
