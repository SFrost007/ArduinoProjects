//Blinking_LED
//turn on the LED for half a second,then off for half a second,repeatedly
//support@sunfounder.com
//www.sunfounder.com
//2015.5.7
/************************************************/
const int ledPin = 9;//the number of the LED pin
const int switchPin = 8;
int mode = LOW;
/************************************************/
void setup()
{
  pinMode(ledPin,OUTPUT);//initialize the digital pin as an output
  pinMode(switchPin,INPUT);
  Serial.begin(9600);
}
/************************************************/
//the loop routine runs over and over again forever
void loop()
{
  int switchPos = digitalRead(switchPin);
  Serial.println(switchPos);
  
  digitalWrite(ledPin,HIGH);//turn the LED on 
  delay(500);               //wait for half a second
  digitalWrite(ledPin,LOW); //turn the LED off
  delay(500);               //wait for half a second
}
/*************************************************/
