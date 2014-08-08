int pirPin1 = 2;
int led1 = 3;
int pirPin2 = 4;
int led2 = 5;
int val1;
int val2;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin1, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(pirPin2, INPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  
  val1 = digitalRead(pirPin1); //read state of the PIR
  val2 = digitalRead(pirPin2); //read state of the PIR
  
  if (val1 == HIGH) {
    Serial.println("Motion in One!"); //if the value read was high, there was motion
    digitalWrite(led1, HIGH);
  }
  else if (val2 == HIGH) {
    Serial.println("Motion in Two!"); //if the value read was high, there was motion
    digitalWrite(led2, HIGH);
  }
    //Serial.println(val);
    delay(1000);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  
//  delay(50);
  
  
}
