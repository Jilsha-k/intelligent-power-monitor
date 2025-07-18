int led=16;
int led1=5;
int buzzer=4;
void setup() {
  // put your setup code here, to run once:
pinMode(led,OUTPUT);
pinMode(led1,OUTPUT);
pinMode(buzzer,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(led,HIGH);
digitalWrite(led1,HIGH);
digitalWrite(buzzer,LOW);
delay(1000);
digitalWrite(led,LOW);
digitalWrite(led1,LOW);
digitalWrite(buzzer,HIGH);
delay(1000);
}
