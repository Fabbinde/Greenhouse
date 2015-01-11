int ledPin = 3;      // LED connected to digital pin 9
int val = 0;         // variable to store the read value

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);   // sets the pin as output
}

void loop()
{
  Serial.println( val);
  if(val == 120) {
   val = 20; 
  }
  analogWrite(ledPin, val);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  val = val+10;
  
  delay(1000);
}
