/*
 Proyect: SCADAcar
 Developed by: Ruy García
 Created: July 22, 2014
 By: Ruy García
 */

// INPUT variables setup:
int pin02PIR1 = 2;

// other variables
volatile int valPIR1 = 0;
int i; // for iteration testing

void setup() {
  Serial.begin(9600);
  //pinMode(pin02PIR1, INPUT);
  attachInterrupt(0, parpadeo, CHANGE);
}

void loop() {
  Serial.println(i++); delay(1000);
}

void parpadeo()
{
    Serial.println("Activado");
    //valPIR1 = !valPIR1;
}

