int ledPin1 = D7; //LED no pino 11
int ledPin2 = D5; //LED no pino 12

int Botao1 = D3; //Botão no pino 2
int Botao2 = D0; //Botão no pino 3

int EstadoBotao1 = 0; //Variável para ler o status do pushbutton
int EstadoBotao2 = 0;


void setup() {
  pinMode(ledPin1, OUTPUT); //Pino do LED será saída
  pinMode(Botao1, INPUT); //Pino com botão será entrada
  pinMode(ledPin2, OUTPUT);
  pinMode(Botao2, INPUT);
}

void loop() {
  EstadoBotao1 = digitalRead(Botao1); /*Novo estado do botão vai ser igual ao que
                                      Arduino ler no pino onde está o botão.
                                      Poderá ser ALTO (HIGH) se o botão estiver
                                      pressionado, ou BAIXO (LOW), se o botão
                                      estiver solto*/
  EstadoBotao2 = digitalRead(Botao2);
   
  
  if(EstadoBotao1 == HIGH) { //Se botão estiver pressionado (HIGH)
    digitalWrite(ledPin1, HIGH); //Acende o LED do pino 11
  }
  else {
    digitalWrite(ledPin1, LOW); //Deixa o LED do pino 11 apagado
  }
  
  if(EstadoBotao2 == LOW) { //Se botão estiver pressionado (HIGH)
    digitalWrite(ledPin2, HIGH); //Acende o LED do pino 12
  }
  else {
    digitalWrite(ledPin2, LOW); //Deixa o LED do pino 12 apagado
  }
  
}
