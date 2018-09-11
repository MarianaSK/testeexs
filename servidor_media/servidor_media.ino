//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
  #include <Adafruit_SSD1306.h>
  
//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     LED0      2           // WIFI Module LED
  #define     LED1      D0          // Connectivity With Client
  #define     LED2      D5          // Connectivity With Client #1
  #define     LED3      D3          // Connectivity With Client #2
  //#define     LED4      D5          // Connectivity With Client #3
  #define     BUTTON    D1          // Connectivity ReInitiate Button
  #define OLED_RESET LED_BUILTIN
  Adafruit_SSD1306 display(OLED_RESET);
//------------------------------------------------------------------------------------
  // Authentication Variables
   char*       ESPssid;              // SERVER WIFI NAME
   char*       ESPpassword;          // SERVER PASSWORD
   String      Message[2];              // VARIABLE RECEIVE DATA FROM OTHER CLIENTS
//------------------------------------------------------------------------------------
  #define     MAXSC     2           // MAXIMUM NUMBER OF CLIENTS
  float       temp1;
  float       temp2;
  WiFiServer  ESPServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  ESPClient[MAXSC];     // THE SERVER CLIENTS (Devices)
  int quantityOfNewClients = 0;
  bool allNewClientsFound = false;
//====================================================================================

  void setup()
  {
    // Setting The Serial Port
    Serial.begin(115200);           // Computer Communication
    
    
    // Setting The Mode Of Pins
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LED1, OUTPUT);          // Indicator For Client  Connectivity
    pinMode(LED2, OUTPUT);          // Indicator For Client  Connectivity
    pinMode(LED3, OUTPUT);          // Indicator For Client  Connectivity
//  pinMode(LED4, OUTPUT);          // Indicator For Client  Connectivity
    pinMode(BUTTON, INPUT_PULLUP);  // Initiate Connectivity
    
    // Print Message Of I/O Setting Progress
    Serial.println();
    Serial.println("I/O Pins Modes Set .... Done");

    // Setting Up A Wifi Access Point
    SetWifi("FVMLearning", "");
  }

//====================================================================================
  
  void loop()
  {    
    if(!allNewClientsFound){
      // Checking If all clients were found
      allNewClientsFound = CheckNewClients();
      Serial.print("Are new clients found?");
      Serial.println(allNewClientsFound);
    }else{    
      // Checking For Available Client Messages
      CalcAverage();  
    }  
  }

//====================================================================================
  
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

  bool CheckNewClients()
  {   
    
    
    if (ESPServer.hasClient())
    {

      Serial.println("The server has client");
      // Read LED0 Switch To Low If High.
      if(digitalRead(LED0) == HIGH) digitalWrite(LED0, LOW);
      
      // Light Up LED1
      digitalWrite(LED1, HIGH);
      
      for(uint8_t i = 0; i < MAXSC; i++)
      {
        //find free/disconnected spot
        if (!ESPClient[i] || !ESPClient[i].connected())
        {
          // Checks If Previously The Client Is Taken
          if(ESPClient[i])
          {
            ESPClient[i].stop();
          }
          // Checks If Clients Connected To The Server
          if(ESPClient[i] = ESPServer.available())
          {
            quantityOfNewClients = quantityOfNewClients + 1;
            Serial.println("New Client: " + ESPClient[i].readStringUntil('\r'));          
          }
        }
      }
      if(quantityOfNewClients == MAXSC)
      {
        allNewClientsFound = true;
      }
      //no free/disconnected spot so reject
      //WiFiClient ESPClient = ESPServer.available();
      //ESPClient.stop();
    }
    else
    {
      // This LED Blinks If No Clients Where Available
      digitalWrite(LED0, HIGH);
      delay(250);
      digitalWrite(LED0, LOW);
      delay(250);
    }
    return allNewClientsFound;
  }

//====================================================================================

  void CalcAverage()
  {
     Serial.println("=================================================================================");
     float sumOfTemperature = 0.0;
     float quantityOfClientRead = 0;
     String stringTemperature;
     float average = 0.0;
     float temperature = 0.0;
     float attemptsByClient = 0;
    //check clients for data
    for(uint8_t i = 0; i < MAXSC; i++)
    {
      Serial.print("Exec : ");
      Serial.println(i);   
      bool disponivel = false;
      if (ESPClient[i] && ESPClient[i].connected())
      {     
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
            temperature = stringTemperature.toFloat();
            Serial.print("Temperature : ");
            Serial.println(temperature); 
            sumOfTemperature = sumOfTemperature + temperature;
            quantityOfClientRead = quantityOfClientRead + 1;                     
       }
       else
       {
          Serial.println("Unknown/Unconnected");  
          Serial.print("Unknown?");
          Serial.println(ESPClient[i] ? "Yes" : "No");
          CheckNewClients();
          if(ESPClient[i]){
            Serial.print("Connected?");
            Serial.println(ESPClient[i].connected() ? "Yes" : "No"); 
             
          }
       }
     }
     
      // If all clients were read then calcute the average of temperature.
      if(quantityOfClientRead == MAXSC)
      {
        Serial.print("Sum temperature :");
        Serial.println(sumOfTemperature); 
        average = sumOfTemperature/quantityOfClientRead;
        Serial.print("Average : ");
        Serial.print(average);
        Serial.println();
        
      }
      Serial.println("=================================================================================");
    }
    
 // =================================================================================

