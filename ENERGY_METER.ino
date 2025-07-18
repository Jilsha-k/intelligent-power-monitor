#include <LiquidCrystal.h> 
LiquidCrystal lcd(2,3, 4, 5, 6, 7);
const int sensorIn = A0;  // pin where the OUT pin from sensor is connected on Arduino 
int mVperAmp = 185;    // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0; 
double Voltage = 0; 
double VRMS = 0;
double AmpsRMS = 0;
double check=0;
float current=0;
int stat_led=6;
int data_led=7; 
int relay1=8; 
int relay2=9;
bool relay1_flag=0,relay2_flag=0,flag1=0,flag2=0;
double sensorValue1 = 0; 
double sensorValue2 = 0; 
int crosscount = 0;
int climb_flag = 0; 
int val[100];
int max_v = 0;
double VmaxD = 0; 
double VeffD = 0; 
double Veff = 0;
int v=0,led_count=0,cnt=0,cnt1=0;
double A;
float W;
float unit=0,x=0;
void setup()
{ 
Serial.begin (9600);
pinMode(relay1,OUTPUT);
pinMode(relay2,OUTPUT);
pinMode(stat_led,OUTPUT); 
pinMode(data_led,OUTPUT);
lcd.begin(16, 2);
lcd.clear(); 
lcd.setCursor(0,0); 
lcd.print(" SMART "); 
lcd.setCursor(0,1);
lcd.print("ENERGY METER"); 
Serial.println ("ACS712 current sensor");
TCCR1A = 0;
TCCR1B = 0;
TCNT1 = 0;
OCR1A = 31250;  // compare match register 16MHz/256/2Hz 
TCCR1B |= (1 << WGM12); // CTC mode
TCCR1B |= (1 << CS12);  // 256 prescaler
TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
interrupts();  // enable all interrupts
}
ISR(TIMER1_COMPA_vect)  // timer compare interrupt service routine
{
  digitalWrite(stat_led, digitalRead(stat_led) ^ 1); // toggle LED pin if(W>10)
  {
    if(Watt>0)
    {
    //x=(Watt/20);
    unit=unit+0.05;
    }
    // Serial.print(unit);
  }
  cnt1++; if(cnt1>3)
  {
  cnt1=0; 
  writeString(String(v)+","+String(current)+","+String(Watt)+","+String(unit));
  }
}
void loop()
{
if(Serial.available()>0)
{
 char msg=Serial.read(); 
 if(msg=='A')
 {
  relay1_flag=1;
  }
  else if(msg=='B')
  {
relay1_flag=0;
  
  }
  else if(msg=='C')
  {
  relay2_flag=1;
  }
  else  if(msg=='D')
  {
  relay2_flag=0;
  }
   
  }
  
}
v=volt_reading(); v=240;
Voltage = getVPP();
VRMS = (Voltage/2.0) *0.707; //root 2 is 0.707 
check=analogRead(A1);
AmpsRMS = (VRMS * 1000)/mVperAmp; 
//Watt = (AmpsRMS*240/1.3);  // 1.3 is an empirical calibration factor 
//A=AmpsRMS;
//W=Watt;
if(relay1_flag==1&&flag1==0)
{
  current=current+0.15;
  flag1=1;
}
if(relay1_flag==0&&flag1==1)
{
  current=current-0.15;
  flag1=0;
}
if(relay2_flag==1&&flag2==0)
{
   current=current+0.22;
   flag2=1;
}
if(relay2_flag==0&&flag2==1)
{
   current=current-0.22;
   flag2=0;
}
Watt=current*v;
/*Serial.print(AmpsRMS); Serial.print(" Amps RMS --- "); Serial.print(Watt);
 Serial.println(" W"); 

 Serial.print("Voltage: "); Serial.println(volt_reading());*/ 
cnt++;
if(cnt>1)
{
cnt=0; led_count++; if(led_count==1)
{
lcd.clear(); lcd.setCursor(0,0);
lcd.print("INPUT VOLTAGE");
lcd.setCursor(5,1); 
lcd.print(String(v)+"V");
}
if(led_count==2)
{
lcd.clear(); lcd.setCursor(0,0);
lcd.print("LOAD CURRENT"); 
lcd.setCursor(5,1); 
lcd.print(String(current)+"A");
}
if(led_count==3)
{
lcd.clear(); 
lcd.setCursor(0,0); 
lcd.print("LOAD POWER"); 
lcd.setCursor(5,1); 
lcd.print(String(Watt)+"W");
}
if(led_count==4)
{
lcd.clear(); lcd.setCursor(0,0);
lcd.print("CONSUMED UNIT"); 
lcd.setCursor(5,1); 
lcd.print(String(unit)+"KWh"); 
led_count=0;
}
}
}
float getVPP()
{
float result;
int readValue;  // value read from the sensor 
int maxValue = 0;   // store max value here
int minValue = 1024;  // store min value here
uint32_t start_time = millis();
while((millis()-start_time) < 1000) //sample for 1 Sec
{
readValue = analogRead(sensorIn);
// see if you have a new maxValue 
if (readValue > maxValue)
{
//record the maximum sensor value 
maxValue = readValue;
}
if (readValue < minValue)
{
//record the minimum sensor value
minValue = readValue;
}
}
result = ((maxValue - minValue) * 5.0)/1024.0;


return result;
}
double volt_reading()
{
for ( int i = 0; i < 100; i++ ) { sensorValue1 = analogRead(A1); if (analogRead(A1) > 511) { val[i] = sensorValue1;
}
else { val[i] = 0;
}
delay(1);
}
max_v = 0;
for ( int i = 0; i < 100; i++ )
{
if ( val[i] > max_v )
{
max_v = val[i];
}
val[i] = 0;
}
if (max_v != 0) { VmaxD = max_v;
VeffD = VmaxD / sqrt(2);
Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2;
}
else { Veff = 0;
}
double fv=Veff-100; VmaxD = 0; delay(100);
return fv;
}
void writeString(String data)
{
for(int i=0;i<data.length();i++)
{
Serial.write(data[i]); delay(10);
//Serial.println(current); 
delay(10);
}
}
