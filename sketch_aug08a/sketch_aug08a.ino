#include <DHT.h>

#define DHTPIN D3
#define DHTTYPE DHT22 //selecionar o tipo de sensor 

DHT dht(DHTPIN,DHTTYPE);

void setup() {
  Serial.begin(115200); 

}

void loop() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature(false);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("*C");
  Serial.print("    ");
  Serial.print("Umidade: ");
  Serial.println(umidade);
  delay(5000);
}
