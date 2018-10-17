//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
  #include <Adafruit_SSD1306.h>
  #include <EEPROM.h>  
  #include <DHT.h>
  #include <TimeLib.h> 
  #include <WiFiUdp.h>

//------------------------------------------------------------------------------------
   //Define I/O Pins
  //#define     LED0      2           // WIFI Module LED
  //#define     LED1      D0          // Connectivity With Client
  //#define     LED2      D5          // Connectivity With Client #1
  //#define     LED3      D          // Connectivity With Client #2
  //#define     LED4      D5          // Connectivity With Client #3
  //#define     BUTTON    D8          // Connectivity ReInitiate Button
  #define OLED_RESET LED_BUILTIN
  #define RELE1            D7
  #define RELE2            D8
  //#define RELE2           D6
  //#define ventilacao      D8
  //#define refrigeracao    D7
  #define DHTPIN          D3
  #define DHTTYPE         DHT22 //selecionar o tipo de sensor
  #define buttonPin_inc   D5   // the pin that the pushbutton is attached to
  #define buttonPin_dec   D6  // the pin that the LED is attached to

  Adafruit_SSD1306 display(OLED_RESET);
  DHT dht(DHTPIN,DHTTYPE);
//------------------------------------------------------------------------------------
  // Authentication Variables
   char*       ESPssid;              // SERVER WIFI NAME
   char*       ESPpassword;          // SERVER PASSWORD
   String      Message[2];              // VARIABLE RECEIVE DATA FROM OTHER CLIENTS
   const char ssid[] = "testeiot";  //  your network SSID (name)
   const char pass[] = "mkuchida";       // your network password
//------------------------------------------------------------------------------------
  #define     MAXSC     2           // MAXIMUM NUMBER OF CLIENTS
  float       temp1;
  float       temp2;
  WiFiServer  ESPServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  ESPClient[MAXSC];     // THE SERVER CLIENTS (Devices)
  int quantityOfNewClients = 0;
  int auxiliarClients;
  bool allNewClientsFound = false;
  bool primeiraLeitura = true;
  bool var_aux;
  float         temperatura;
  float         umidade;
  int  buttonPushCounter     = 0;   // counter for the number of button presses
  bool buttonState_inc       = 0;         // current state of the button
  bool lastButtonState_inc   = 0;     // previous state of the button
  bool buttonState_dec       = 0;         // current state of the button
  bool lastButtonState_dec   = 0;     // previous state of the button
  float  setPoint            = 23.00;
  float temperaturaIdeal     = 23.00;
  int  newsetPoint           = 0;
  unsigned long tempo_espera;
  float average;
  String cliente;
  //--------------------------
 
//====================================================================================
int getMinimumSamplingPeriod() { 
    return 2000; 
  }
//====================================================================================
  // NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov


const int timeZone = -3;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
//============================================================================
  void setup()
  {
    // Setting The Serial Port
    Serial.begin(115200);           // Computer Communication
    ConfigDisplay();
    
    pinMode(buttonPin_inc, INPUT);
    pinMode(buttonPin_dec, INPUT);
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RELE1, OUTPUT);
    pinMode(RELE2, OUTPUT);
    /*pinMode(ventilacao, OUTPUT);
    digitalWrite(ventilacao, HIGH); 
    pinMode(refrigeracao, OUTPUT);*/
    Serial.print("Temperatura atual (Set Point): ");
    Serial.println(setPoint);
    
    // Setting The Mode Of Pins
    //pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    //pinMode(LED1, OUTPUT);          // Indicator For Client  Connectivity
   // pinMode(LED2, OUTPUT);          // Indicator For Client  Connectivity
    //pinMode(LED3, OUTPUT);          // Indicator For Client  Connectivity
//  pinMode(LED4, OUTPUT);          // Indicator For Client  Connectivity
    //pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
    
    // Print Message Of I/O Setting Progress
    Serial.println();
    Serial.println("I/O Pins Modes Set .... Done");

    // Setting Up A Wifi Access Point
    SetWifi("FVMLearning", "");
    while (!Serial) ; // Needed for Leonardo only
    delay(250);
    Serial.println("TimeNTP Example");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if(!allNewClientsFound){
        Serial.println("Não há clientes");
        Serial.println("Botões disponíveis");
      }
    }
    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    
    
  }
  time_t prevDisplay = 0; // when the digital clock was displayed

//====================================================================================
  // ********ponto certo**************
  void loop()
  {  
    CheckNewClients();
    Serial.print("Todos os clientes encontrados: [0]FALSE [1]TRUE: ");
    Serial.println(allNewClientsFound);
    Serial.print("Quantidade de clientes conectados: ");
    Serial.println(quantityOfNewClients);
    if(!allNewClientsFound){
      // Checking If all clients were found
      medirTemperaturaUmidade();
      tomada_decisao_manual();
      
      //mostrar_temp_umid();
    }
    
    else{    
      // Checking For Available Client Messages
      exibe_display_auto();
      CalcAverage();  
      
    }  
    if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
      onoff_led();
    }
  }
  }

//====================================================================================
  void digitalClockDisplay(){
  // digital clock display of the time
  name_weekday();
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 
  
}
void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
  Serial.print('0');
  Serial.print(digits);
}
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket("pool.ntp.br", 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

// --- DIA DA SEMANA ---
void name_weekday(){
  
  if (weekday() == 1 ){
    Serial.print("Domingo");
    Serial.print(" ");
  }  

  if (weekday() == 2 ){
    Serial.print("Segunda-feira");
    Serial.print(" ");
  }
    
  if (weekday() == 3 ){
    Serial.print("Terça-feira");
    Serial.print(" ");
  }
    
  if (weekday() == 4 ){
    Serial.print("Quarta-feira");
    Serial.print(" ");
  }
    
  if (weekday() == 5 ){
    Serial.print("Quinta-feira");
    Serial.print(" ");
  }
    
  if (weekday() == 6 ){
    Serial.print("Sexta-feira");
    Serial.print(" ");
  }
    
  if (weekday() == 7 ){
    Serial.print("Sábado");
    Serial.print(" ");
  }
    
}
void onoff_led(){
  if ((hour() == 16) && (minute() >= 34) && (minute() <= 35) && (weekday() >= 2) && (weekday() <= 6)){
    digitalWrite(LED_BUILTIN,0);//ACENDE LED
    digitalWrite(RELE1,0);
    digitalWrite(RELE2,0);
    //delay(1000);
    //digitalWrite(RELE2,HIGH);
  }
  if((hour() == 16) && (minute() > 39) && (minute() <= 40) && (weekday() >= 2) && (weekday() <= 6)){
    digitalWrite(LED_BUILTIN,1);//APAGA LED
    digitalWrite(RELE1,1);
    digitalWrite(RELE2,0);
    delay(10000);
    //digitalWrite(RELE2,HIGH);
  }
  delay(1000);
}
  void SetWifi(char* Name, char* Password)
  {
    // Stop Any Previous WIFI
    WiFi.disconnect();

    // Setting The Wifi Mode
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WIFI Mode : AccessPoint Station");
    
    // Setting The AccessPoint Name & Password
    ESPssid      = Name;
    ESPpassword  = Password;
    
    // Starting The Access Point
    WiFi.softAP(ESPssid, ESPpassword);
    Serial.println("WIFI < " + String(ESPssid) + " > ... Started");
    
    // Wait For Few Seconds
    delay(500);
    
    // Getting Server IP
    IPAddress IP = WiFi.softAPIP();
    
    // Printing The Server IP Address
    Serial.print("AccessPoint IP : ");
    Serial.println(IP);

    // Printing MAC Address
    Serial.print("AccessPoint MC : ");
    Serial.println(String(WiFi.softAPmacAddress()));

    // Starting Server
    ESPServer.begin();
    ESPServer.setNoDelay(true);
    Serial.println("Server Started");
  }

//====================================================================================
void tomada_decisao_manual(){
  /*if(setPoint>=22 && setPoint<=24){
    digitalWrite(LED_BUILTIN, LOW); //sistema de ventilação
  }
  else if (setPoint<22){
    digitalWrite(LED_BUILTIN, HIGH); //sistema de refrigeração
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }*/
  exibe_display_manual();
  if(setPoint>=temperaturaIdeal || temperatura==setPoint){
    //TESTE COM LED
    //digitalWrite(ventilacao, HIGH);
    //digitalWrite(refrigeracao, LOW); //LED verde 
    digitalWrite(RELE1,LOW);
    digitalWrite(RELE2,LOW);
    display.drawCircle(100,45,5,WHITE); //ESQ - VENT
    display.fillCircle(40,45,5,WHITE); //DIR - REFRIG
    
    display.setTextSize(1);
    display.setCursor(30,55);
    display.print("vent");
    display.setTextSize(1);
    display.setCursor(75,55);
    display.print("refrig");
    display.display();
    delay(250);
    //TESTE COM LED E RELE
    //digitalWrite(RELE1, HIGH);
    //digitalWrite(RELE2, LOW);
    
  }
  if(setPoint<temperaturaIdeal){
    //TESTE COM LED
    //digitalWrite(ventilacao, LOW);
    //digitalWrite(refrigeracao, HIGH); //LED vermelho
    digitalWrite(RELE1,LOW);
    digitalWrite(RELE2,HIGH);
    display.fillCircle(40,45,5,WHITE); //ESQ - VENT
    display.fillCircle(100,45,5,WHITE); //DIR - REFRIG
    display.setTextSize(1);
    display.setCursor(30,55);
    display.print("vent");
    display.setTextSize(1);
    display.setCursor(75,55);
    display.print("refrig");
    display.display();
    delay(250);
    //TESTE COM LED E RELE
    //digitalWrite(RELE2, HIGH);
    //digitalWrite(RELE1, LOW);
  }
  
 /* else{
    digitalWrite(ventilacao, HIGH);  
  }*/
}
/*void tomada_decisao_auto(){
  if(setPoint>=22 && setPoint<=24){
    digitalWrite(LED_BUILTIN, LOW); //sistema de ventilação
  }
  else if (setPoint<22){
    digitalWrite(LED_BUILTIN, HIGH); //sistema de refrigeração
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  digitalWrite(ventilacao, HIGH); //LED verde
  if(average<=21.50){
    digitalWrite(refrigeracao, LOW); //LED verde 
  }
  if(average>=22.50){
    digitalWrite(refrigeracao, HIGH); //LED vermelho
  }
}*/
//====================================================================================
void medirTemperaturaUmidade() {
  if(primeiraLeitura){
     delay(getMinimumSamplingPeriod());
     primeiraLeitura = false;
  }
  
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  Serial.print("Temperatura ---- modo manual: ");
  Serial.println(temperatura);
  Serial.print("Setpoint: ");
  Serial.println(setPoint);  
  
  
  if(temperatura>=15 && temperatura<31){
        Serial.println("entrada na condição de botões disponíveis"); 
        //while(lastButtonState_inc==LOW || lastButtonState_dec==LOW){
          config_button();
          //retorno_button();
         yield();
         //tempo_espera=millis();
       
       // }
        
  }
  
  else{
    setPoint=temperaturaIdeal;
    Serial.println("Não há medição de temperatura");
    Serial.print("Temperatura ajustada para ");
    Serial.print(setPoint);
    Serial.println(" C");
    Serial.println("Botões desabilitado");
  }
  
  //delay(5000);
  //tomada_decisao_manual();
  //yield();
}
//====================================================================================
/*void mostrar_temp_umid(){
  exibe_display("Temperatura ", (temperatura)," C");
  //exibe_display("Umidade ",(umidade)," %");
}*/
//*******************
//EXIBE TEMPERATURA E UMIDADE NO DISPLAY
void exibe_display_manual(){
  display.clearDisplay(); //limpa o display
  display.setTextSize(1); //1a linha
  display.setCursor(0,0);
  display.print("MODO MANUAL");
  display.setTextSize(1); //1a linha
  display.setCursor(0,17);
  display.print("Temperatura ");
  display.setTextSize(1); // 2a linha
  display.setCursor(0,27);
  display.print(temperatura);
  display.setTextSize(1);
  display.print(" C");
  display.setTextSize(1); //1a linha
  display.setCursor(80,17);
  display.print("Setpoint ");
  display.setTextSize(1); // 2a linha
  display.setCursor(80,27);
  display.print(setPoint);
  display.setTextSize(1);
  display.print(" C");
  /*display.drawCircle(40,45,5,WHITE); //ESQ - VENT
  display.fillCircle(100,45,5,WHITE); //DIR - REFRIG
  display.setTextSize(1);
  display.setCursor(40,55);
  display.print("vent"); 
  display.setTextSize(1);
  display.setCursor(100,55);
  display.print("refrig");
  display.display();
  delay(250);*/
}

void exibe_display_auto(){
  
  display.clearDisplay(); //limpa o display
  display.setTextSize(1); //1a linha
  display.setCursor(0,0);
  display.print("MODO AUTOMATICO");
  display.setTextSize(1); //1a linha
  display.setCursor(0,17);
  display.print("Temp ideal");
  display.setTextSize(1); // 2a linha
  display.setCursor(0,27);
  display.print(temperaturaIdeal);
  display.setTextSize(1);
  display.print(" C");
  display.setTextSize(1); //1a linha
  display.setCursor(65,17);
  display.print("Temp media");
  display.setTextSize(1); // 2a linha
  display.setCursor(65,27);
  display.print(average);
  display.setTextSize(1);
  display.print(" C");
  
}

void ConfigDisplay(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();  
  
}
//====================================================================================
void temperatura_faixa_operacao_inc(){
  if((setPoint >= 15) && (setPoint < 31)){
        Serial.print("ajuste de temperatura: ");
        Serial.println(setPoint);
        
  } 
}

void temperatura_foradafaixa(){
  if(setPoint < 15){
        setPoint=15;
        Serial.print("ajuste de temperatura: ");
        Serial.println(setPoint);
      
      }
   if(setPoint >= 31){
        setPoint=31;
        Serial.print("ajuste de temperatura: ");
        Serial.println(setPoint);
   }
}
//===================================================================================

void config_button(){
   //Serial.println("--- Botões disponíveis ---");
   //buttonState_inc == LOW;
   //buttonState_dec == LOW;
   buttonState_inc = digitalRead(buttonPin_inc); 
   buttonState_dec = digitalRead(buttonPin_dec);
   //setPoint    = setPoint + variavel_auxiliar;
  // compare the buttonState to its previous state
  // --- CONDIÇÃO BOTÃO DE INCREMENTO ---
  if(buttonState_inc != lastButtonState_inc){
    // if the state has changed, increment the counter
    if(buttonState_inc == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.print("número de vezes em que o botão foi pressionado: ");
      Serial.println(buttonPushCounter);
      
      setPoint++;
      temperatura_faixa_operacao_inc();
      temperatura_foradafaixa();      
      
    // Delay a little bit to avoid bouncing
    delay(50);
    tomada_decisao_manual();
    
    }
    
  // save the current state as the last state, for next time through the loop
  lastButtonState_inc = buttonState_inc;
  
  }

  // --- CONDIÇÃO BOTÃO DECREMENTO ---
  if(buttonState_dec != lastButtonState_dec){
    // if the state has changed, increment the counter
    if(buttonState_dec == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.print("número de vezes em que o botão foi pressionado: ");
      Serial.println(buttonPushCounter);
      
      setPoint--;
      temperatura_faixa_operacao_inc();
      temperatura_foradafaixa();      
      
    // Delay a little bit to avoid bouncing
    delay(50);
    tomada_decisao_manual();
    
    }
  // save the current state as the last state, for next time through the loop
  lastButtonState_dec = buttonState_dec;
  
  }
  
  
}
//====================================================================================

  bool CheckNewClients()
  {   
    if(quantityOfNewClients<MAXSC){
    if (ESPServer.hasClient())
    {
      
      Serial.println("----The server has client----");
      // Read LED0 Switch To Low If High.
      /*if(digitalRead(LED0) == HIGH) digitalWrite(LED0, LOW);
      
      // Light Up LED1
      digitalWrite(LED1, HIGH);*/
      
      for(uint8_t i = 0; i < MAXSC; i++)
      {
        //find free/disconnected spot
        if (!ESPClient[i] || !ESPClient[i].connected())
        {
          Serial.println("**************************************");
          // Checks If Previously The Client Is Taken
          if(ESPClient[i])
          {
            Serial.println("entra no código?");
            ESPClient[i].stop();
          }
          // Checks If Clients Connected To The Server
          if(ESPClient[i] = ESPServer.available())
          {
            quantityOfNewClients = quantityOfNewClients + 1;
            cliente=ESPClient[i].readStringUntil('\r');
            Serial.println("New Client: " + cliente);                   
          }
        }
      }
      //medirTemperaturaUmidade();
      //config_button();
      if(quantityOfNewClients == MAXSC)
      {
        Serial.println("The server has client");
        allNewClientsFound = true;
      }
      //no free/disconnected spot so reject
      //WiFiClient ESPClient = ESPServer.available();
      //ESPClient.stop();
    }
    else{
      if(quantityOfNewClients < MAXSC)
      {
        Serial.println("The server doesn't have all clients");
        allNewClientsFound = false;        
      }
    }
   }
    return allNewClientsFound;
  }

//====================================================================================

  void CalcAverage()
  {
     Serial.println("media");
     Serial.println("=================================================================================");
     float sumOfTemperature = 0.0;
     int quantityOfClientRead = 0;
     String stringTemperature;
     //float average = 0.0;
     float temperature = 0.0;
     float attemptsByClient = 0; 
    //check clients for data
    for(uint8_t i = 0; i < MAXSC; i++)
    {
      cliente=ESPClient[i].readStringUntil('\r');
      //Serial.println(cliente);
      Serial.print("Exec : ");
      Serial.println(i);   
      bool disponivel = false;
      if (ESPClient[i] && ESPClient[i].connected())
      {     
             Serial.println(cliente+"conectado");
            // Waiting for receiving client message 
            while(!ESPClient[i].available() && attemptsByClient != 5){
              delay(2000);    
              attemptsByClient = attemptsByClient + 1;
            }
            if(attemptsByClient == 5){
              // Temperature was not received by the server
              continue;
            }
            stringTemperature = ESPClient[i].readStringUntil('\r');  
            ESPClient[i].flush();
            ESPClient[i].read();
            temperature = cliente.toFloat();
            Serial.print("Temperature: ");
            Serial.println(temperature); 
            
            sumOfTemperature = sumOfTemperature + temperature;
            quantityOfClientRead = quantityOfClientRead + 1;                     
       }
       else 
       {
          Serial.println("desconectado");
          medirTemperaturaUmidade();
          //tomada_decisao_manual();
          Serial.println("Unknown/Unconnected---------");  
          Serial.println("Unknown?");
          
          Serial.println(ESPClient[i] ? "Yes" : "No");

          if(ESPClient[i]){
            Serial.print("Connected?");
            Serial.println(ESPClient[i].connected() ? "Yes" : "No"); 
           
          }
          
       }
     }
     
      // If all clients were read then calcute the average of temperature.
      if(quantityOfClientRead == MAXSC)
      {
        allNewClientsFound = true;
        Serial.println("todos os clientes conectados");
        Serial.print("Sum temperature :");
        Serial.println(sumOfTemperature); 
        average = sumOfTemperature/quantityOfClientRead;
        if(!(average>=15 && average<=31)){
          average=temperaturaIdeal;
          Serial.print("Erro na leitura, média de temperatura ajustada para ");
          Serial.print(average);
          Serial.println(" C");
        }
        Serial.print("Average : ");
        Serial.print(average);
        Serial.println();
        //setPoint=average;
        Serial.print("Set Point: ");
        Serial.println(setPoint);
        //temperatura_faixa_operacao_inc();
        //temperatura_foradafaixa();  
        //tomada_decisao_manual();  
         //LED verde
        if(average<=temperaturaIdeal){
          //digitalWrite(refrigeracao, LOW); //LED verde 
          //digitalWrite(ventilacao, HIGH);
          digitalWrite(RELE1, LOW);
          digitalWrite(RELE2, LOW);
          display.drawCircle(100,45,5,WHITE); //ESQ - VENT
          display.fillCircle(40,45,5,WHITE); //DIR - REFRIG
          display.setTextSize(1);
          display.setCursor(30,55);
          display.print("vent");
          display.setTextSize(1);
          display.setCursor(85,55);
          display.print("refrig");
          display.display();
          delay(250);
        }
        if(average>temperaturaIdeal){
          //digitalWrite(ventilacao, LOW);
          //digitalWrite(refrigeracao, HIGH); //LED vermelho
          digitalWrite(RELE1, LOW);
          digitalWrite(RELE2, HIGH);
          display.fillCircle(40,45,5,WHITE); //ESQ - VENT
          display.fillCircle(100,45,5,WHITE); //DIR - REFRIG
          display.setTextSize(1);
          display.setCursor(30,55);
          display.print("vent");
          display.setTextSize(1);
          display.setCursor(85,55);
          display.print("refrig");
          display.display();
          delay(250);
        }
      }
  if (quantityOfClientRead < MAXSC)
      {
        if(quantityOfNewClients==2){
          quantityOfNewClients=quantityOfClientRead--;
          auxiliarClients=quantityOfNewClients;
        }
        if(auxiliarClients==1){
          quantityOfNewClients--;
          auxiliarClients=quantityOfNewClients;
        }
        
        Serial.println("um ou todos os clientes desconectado(s)");
        Serial.println("Unknown/Unconnected");  
        Serial.println("Unknown?");
        allNewClientsFound = false;
        /*ESPServer.begin();
        ESPServer.setNoDelay(true);
        Serial.println("Server Started");*/
      }
      //esse trecho possui alguns problemas, mas está quase pronto

      Serial.println("=================================================================================");
     }
    
 // =================================================================================
