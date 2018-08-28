//const int pinoBotao = D2; //PINO DIGITAL UTILIZADO PELO PUSH BUTTON
//const int pinoLed = D1; //PINO DIGITAL UTILIZADO PELO LED

//initialize and declare variables
const int ledPin = D1; //led attached to this pin
const int buttonPin = D2; //push button attached to this pin
 
int buttonState = LOW; //this variable tracks the state of the button, low if not pressed, high if pressed
int ledState = -1; //this variable tracks the state of the LED, negative if off, positive if on
 
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 250;    // the debounce time; increase if the output flickers
 
 
void setup() {
 
  //set the mode of the pins...
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
}//close void setup
 
 
void loop() {
 
  //sample the state of the button - is it pressed or not?
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
 
}//close void loop
