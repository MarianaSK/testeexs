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
  #define RELE2            D5
  //#define RELE2           D6
  //#define ventilacao      D8
  //#define refrigeracao    D7
  #define DHTPIN          D3
  #define DHTTYPE         DHT22 //selecionar o tipo de sensor
  #define buttonPin_inc   D8    // the pin that the pushbutton is attached to
  #define buttonPin_dec   D6    // the pin that the LED is attached to
  #define pinoPulsador    D4    //PINO DIGITAL UTILIZADO PELO PULSADOR
  
  Adafruit_SSD1306 display(OLED_RESET);
  DHT dht(DHTPIN,DHTTYPE);
//------------------------------------------------------------------------------------
  // Authentication Variables
   char*       ESPssid;              // SERVER WIFI NAME
   char*       ESPpassword;          // SERVER PASSWORD
   String      Message[2];              // VARIABLE RECEIVE DATA FROM OTHER CLIENTS
   const char  ssid[] = "Opus-Guest";  //  your network SSID (name)
   const char  pass[] = "";       // your network password
//------------------------------------------------------------------------------------
  #define     MAXSC     2           // MAXIMUM NUMBER OF CLIENTS
  float       temp1;
  float       temp2;
  WiFiServer  ESPServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  ESPClient[MAXSC];     // THE SERVER CLIENTS (Devices)
  int         quantityOfNewClients = 0;
  int         auxiliarClients;
  bool        allNewClientsFound = false;
  bool        primeiraLeitura = true;
  bool        var_aux;
  float       temperatura;
  float       umidade;
  int         buttonPushCounter     = 0;   // counter for the number of button presses
  bool        buttonState_inc       = 0;         // current state of the button
  bool        lastButtonState_inc   = 0;     // previous state of the button
  bool        buttonState_dec       = 0;         // current state of the button
  bool        lastButtonState_dec   = 0;     // previous state of the button
  float       setPoint            = 22.00;
  float       temperaturaIdeal     = 22.00;
  int         newsetPoint           = 0;
  unsigned long tempo_espera;
  float       average;
  String      cliente[2];
  String      serverMAC;
  float       temperature[2];
  String      stringTemperature;
  //String poscliente[3][2];
  float       mediaCliente;
  int         segundoPostadoC1;
  int         segundoPassadoC1=0;
  int         segundoPostadoC2;
  int         segundoPassadoC2=0;
  int         tempoPermitido=30;
  int         tempoLimite;
  int quantityOfClientRead = 0;
  //String tabelaCliente[20][2];
  //--------------------------
 
//====================================================================================
int getMinimumSamplingPeriod() { 
    return 2000; 
  }
//====================================================================================
  // NTP Servers:
IPAddress timeServer(200,186,125,195); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov


const int timeZone = -2;     // Central European Time
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
    dht.begin();
    ConfigDisplay();
    
    pinMode(buttonPin_inc, INPUT);
    pinMode(buttonPin_dec, INPUT);
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RELE1, OUTPUT);
    pinMode(RELE2, OUTPUT);
    pinMode(pinoPulsador, INPUT_PULLUP); //DEFINE A PORTA COMO ENTRADA E ATIVA O RESISTOR INTERNO DE PULL UP
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
        //Serial.println("Botões disponíveis");
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
    /*if(!allNewClientsFound){
      // Checking If all clients were found
      medirTemperaturaUmidade();
      tomada_decisao_manual();
      
      //mostrar_temp_umid();
    }
    
    else{    
      // Checking For Available Client Messages
      exibe_display_auto();
      CalcAverage();  
      
    }  */
    botaoSelecao();
    if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
      onoff_led();
    }
  }
  //tabelaCliente();
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
  Udp.beginPacket("c.st1.ntp.br", 123); //NTP requests are to port 123
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
  if ((hour() == 15) && (weekday() >= 2) && (weekday() <= 6)){ //funçao desliga
    desligaTermostato();
    //digitalWrite(RELE2,HIGH);
  }
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
    serverMAC=String(WiFi.softAPmacAddress());
    Serial.print("AccessPoint MC : ");
    Serial.println(serverMAC);

    // Starting Server
    ESPServer.begin();
    ESPServer.setNoDelay(true);
    Serial.println("Server Started");
  }

//====================================================================================
void tomada_decisao_manual(){
 
    exibe_display_manual();
  
        if(temperatura<=setPoint){
        //TESTE COM LED
        //digitalWrite(ventilacao, HIGH);
        //digitalWrite(refrigeracao, LOW); //LED verde 
        
        digitalWrite(RELE1,HIGH);
        digitalWrite(RELE2,HIGH);
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
        Serial.print("desliga refrig");
        //TESTE COM LED E RELE
        //digitalWrite(RELE1, HIGH);
        //digitalWrite(RELE2, LOW);
        
      }
      else{
        digitalWrite(RELE1, LOW);
        digitalWrite(RELE2, HIGH);
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
        Serial.print("liga refrig");
      }
  
}

//====================================================================================
void medirTemperaturaUmidade() {
  if(primeiraLeitura){
     delay(getMinimumSamplingPeriod());
     primeiraLeitura = false;
  }
  
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  Serial.print("Server MAC: ");
  Serial.println(serverMAC);
  Serial.print("Temperatura ---- modo manual: ");
  Serial.println(temperatura);
  Serial.print(" Setpoint: ");
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
    temperatura=temperaturaIdeal;
    Serial.println("Não há medição de temperatura");
    Serial.print("Temperatura ajustada para ");
    Serial.print(temperatura);
    Serial.println(" C");
  }
  
  
}
//====================================================================================

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
  display.print(mediaCliente);
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
   
   buttonState_inc = digitalRead(buttonPin_inc); 
   buttonState_dec = digitalRead(buttonPin_dec);
   
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
            cliente[i]=ESPClient[i].readStringUntil('\r');
            Serial.println("New Client: " + cliente[i]);                   
          }
        }
      }
      
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
     float attemptsByClient = 0; 
     temperatura = dht.readTemperature(false);
    //check clients for data
    for(uint8_t i = 0; i < MAXSC; i++)
    {
      cliente[i]=ESPClient[i].readStringUntil(' ');
            
      
      Serial.print("Exec : ");
      Serial.println(i);   
      bool disponivel = false;
      if (ESPClient[i] && ESPClient[i].connected())
      {     
             Serial.println(cliente[i]+" conectado");
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
            temperature[i] = stringTemperature.toFloat();
            Serial.print("Temperature: ");
            Serial.println(temperature[i]); 
            
            //sumOfTemperature = sumOfTemperature + temperature[i];
            quantityOfClientRead = quantityOfClientRead + 1;  
            //tabelaCliente();                 
       }
       else 
       {
          Serial.println("desconectado");
          //medirTemperaturaUmidade();
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
      //if(quantityOfClientRead == MAXSC)
      //{
        //allNewClientsFound = true;
        //Serial.println("todos os clientes conectados");
        //Serial.print("Sum temperature :");
        //Serial.println(sumOfTemperature); 
        //average = sumOfTemperature/quantityOfClientRead;
        
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
        //allNewClientsFound = false;
        /*ESPServer.begin();
        ESPServer.setNoDelay(true);
        Serial.println("Server Started");*/
      }
      //esse trecho possui alguns problemas, mas está quase pronto

      Serial.println("=================================================================================");
      tabelaCliente();
      if(!(mediaCliente>=15 && mediaCliente<=31)){
          mediaCliente=temperaturaIdeal;
          Serial.print("Erro na leitura, média de temperatura ajustada para ");
          Serial.print(mediaCliente);
          Serial.println(" C");
        }
        Serial.print("Average : ");
        Serial.print(mediaCliente);
        Serial.println();
        //setPoint=average;
        Serial.print("Set Point: ");
        Serial.println(setPoint);
        //temperatura_faixa_operacao_inc();
        //temperatura_foradafaixa();  
        //tomada_decisao_manual();  
         //LED verde
        if(mediaCliente<=temperaturaIdeal){
          //digitalWrite(refrigeracao, LOW); //LED verde 
          //digitalWrite(ventilacao, HIGH);
          digitalWrite(RELE1, HIGH);
          digitalWrite(RELE2, HIGH);
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
        if(mediaCliente>temperaturaIdeal){
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
  
    
 // =================================================================================
 // ESTRUTURA DE DADOS
  void tabelaCliente(){
     String cliente1;
     String cliente2;
     int hora;
     int minuto;
     int segundo;
     String stringHora;
     String stringMinuto;
     String stringSegundo;

     float controleTemperatura;
     float controleCliente1;
     float controleCliente2;
    
    hora=hour();
    minuto=minute();
    segundo=second();
    stringHora=String(hora);
    stringMinuto=String(minuto);
    stringSegundo=String(segundo);
    if(ESPClient[0].connected() && ESPClient[1].connected()){
    String poscliente[3][5]={
      {serverMAC, String(temperatura), stringHora, stringMinuto, stringSegundo},
      {cliente[0], String(temperature[0]), stringHora, stringMinuto, stringSegundo},
      {cliente[1], String(temperature[1]), stringHora, stringMinuto, stringSegundo}
    }; 
    
    Serial.print(poscliente[0][0]+" ");
    Serial.print(poscliente[0][1]+" ");
    Serial.print(poscliente[0][2]+":");
    Serial.print(poscliente[0][3]+":");
    Serial.println(poscliente[0][4]);
    Serial.print(poscliente[1][0]+" ");
    Serial.print(poscliente[1][1]+" ");
    Serial.print(poscliente[1][2]+":");
    Serial.print(poscliente[1][3]+":");
    Serial.println(poscliente[1][4]);
    Serial.print(poscliente[2][0]+" ");
    Serial.print(poscliente[2][1]+" ");
    Serial.print(poscliente[2][2]+":");
    Serial.print(poscliente[2][3]+":");
    Serial.println(poscliente[2][4]);
    delay(500);
  
    mediaCliente=((poscliente[0][1].toFloat()+poscliente[1][1].toFloat()+poscliente[2][1].toFloat())/3);
    Serial.print("Media: ");
    Serial.println(mediaCliente);
    
    Serial.println("=================================================================================");
    }
     
    if(ESPClient[0].connected() && !ESPClient[1].connected()){
    //if (!ESPClient[0].connected() || !ESPClient[1].connected()){
        String poscliente[2][5]={
        {serverMAC, String(temperatura), stringHora, stringMinuto, stringSegundo},
        {cliente[0], String(temperature[0]), stringHora, stringMinuto, stringSegundo},
        //{cliente[1], String(temperature[1]), stringHora, stringMinuto, stringSegundo}
      };
      
      Serial.print(poscliente[0][0]+" ");
      Serial.print(poscliente[0][1]+" ");
      Serial.print(poscliente[0][2]+":");
      Serial.print(poscliente[0][3]+":");
      Serial.println(poscliente[0][4]);
      Serial.print(poscliente[1][0]+" ");
      Serial.print(poscliente[1][1]+" ");
      Serial.print(poscliente[1][2]+":");
      Serial.print(poscliente[1][3]+":");
      Serial.println(poscliente[1][4]);
      delay(500);
    
      mediaCliente=((poscliente[0][1].toFloat()+poscliente[1][1].toFloat())/2);
      Serial.print("Media: ");
      Serial.println(mediaCliente);
      
      Serial.println("=================================================================================");
      //}
    }
    
    if(!ESPClient[0].connected() && !ESPClient[1].connected()){
      String poscliente[1][5]={
      {serverMAC, String(temperatura), stringHora, stringMinuto, stringSegundo},
      //{cliente[0], String(temperature[0]), stringHora, stringMinuto, stringSegundo},
      //{cliente[1], String(temperature[1]), stringHora, stringMinuto, stringSegundo}
    };
    
    Serial.print(poscliente[0][0]+" ");
    Serial.print(poscliente[0][1]+" ");
    Serial.print(poscliente[0][2]+":");
    Serial.print(poscliente[0][3]+":");
    Serial.println(poscliente[0][4]);
    delay(500);
  
    mediaCliente=poscliente[0][1].toFloat();
    Serial.print("Media: ");
    Serial.println(mediaCliente);
    
    Serial.println("=================================================================================");
    }
  }
  

void botaoSelecao(){
  if((digitalRead(pinoPulsador) == 0)){ //SE A LEITURA DO PULSADOR FOR IGUAL A 0 "E" VARIÁVEL STATUS IGUAL A 0, FAZ  
    //digitalWrite(pinoRele,HIGH); //RELÉ LIGADO
    //status = 1; //VARIÁVEL RECEBE O VALOR 1
    //st = "ACESA"; //VARIÁVEL RECEBE O TEXTO
    medirTemperaturaUmidade();
    tomada_decisao_manual();
    //delay(1000); //INTERVALO DE 200 MILISSEGUNDOS
  } //SENÃO, FAZ
    if((digitalRead(pinoPulsador) == 1)){ //SE  A LEITURA DO PULSADOR FOR IGUAL A 0 "E" VARIÁVEL STATUS IGUAL A 1, FAZ 
    //digitalWrite(pinoRele,LOW); //RELÉ DESLIGADO
    //status = 0; //VARIÁVEL RECEBE O VALOR 0
    //st = "APAGADA";  //VARIÁVEL RECEBE O TEXTO
    exibe_display_auto();
    CalcAverage();
    
    //delay(1000); //INTERVALO DE 200 MILISSEGUNDOS
    }
}

void desligaTermostato(){
    digitalWrite(RELE1,1);
    digitalWrite(RELE2,0);
    delay(3600000);
}


