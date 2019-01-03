#include <NewPing.h>

/*
  motA | motB  | Evento
  -----|-------|----------------------
  LOW  | LOW   | fermo
  LOW  | HIGH  | Movimento in un senso
  HIGH | LOW   | Movimento senso opposto
  HIGH | HIGH  | Fermo
  */

//SET PIN
static int triggerPin = 3;
static int echoPin = 4;
static int ledPin = 2;
static int sensorPin = A0;

//SET VARIABILI
static int defaultVel = 150;
static int triggerBack = 30;

//PIN MOTORE SINISTRO
static int motsxa= 12;
static int motsxb = 13;
static int pinVelsx = 11;

//PIN MOTORE DESTRO
static int motdxa = 8;
static int motdxb = 10;
static int pinVeldx = 9;

//CREAZIONE VARIABILI
int distanza;
int luminosita;
int triggerLed;
char input;

NewPing sonar (triggerPin, echoPin, 300);


//FUNCTIONS

void brake() {
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, LOW);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, LOW);
}

void forward(int velocity) {
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, HIGH);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, HIGH);
  analogWrite(pinVelsx, velocity);
  analogWrite(pinVeldx, (velocity - 10));
}

void backward(int velocity) {
  digitalWrite(motsxa, HIGH);
  digitalWrite(motsxb, LOW);
  digitalWrite(motdxa, HIGH);
  digitalWrite(motdxb, LOW);
  analogWrite(pinVelsx, velocity);
  analogWrite(pinVeldx, (velocity - 10));
}

void left(int velocity) {
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, HIGH);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, HIGH);
  analogWrite(pinVelsx, (velocity - 20));
  analogWrite(pinVeldx, (velocity + 30));
}

void right(int velocity){
  digitalWrite(motsxa, LOW);
  digitalWrite(motsxb, HIGH);
  digitalWrite(motdxa, LOW);
  digitalWrite(motdxb, HIGH);
  analogWrite(pinVelsx, (velocity + 20));
  analogWrite(pinVeldx, (velocity - 30));
}





void setup() {
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);
  pinMode(motsxa, OUTPUT);
  pinMode(motsxb, OUTPUT);
  pinMode(pinVelsx, OUTPUT);
  pinMode(motdxa, OUTPUT);
  pinMode(motdxb, OUTPUT);
  pinMode(pinVeldx, OUTPUT);
  
  luminosita = 0;
  for (int i=0; i<10; i++) {
    luminosita = luminosita + analogRead(sensorPin);
    delay(250);
    }
  triggerLed = (luminosita/10) - 50;
}





void loop() {
  //CONTROLLO LED
  luminosita = analogRead(sensorPin);
  if (luminosita < triggerLed)
    digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);


  //CONTROLLO MOTORI
  distanza = sonar.ping_cm();
  if(distanza < triggerBack && distanza != 0)
    brake();
  if(Serial.available()) {
    input = Serial.read();
    if(input == 'a')
      forward(defaultVel);
    if(input == 'i')
      backward(defaultVel);
    if(input == 's')
      left(defaultVel);
    if(input == 'd')
      right(defaultVel);
    if(input == 's')
      brake();
  }
}
