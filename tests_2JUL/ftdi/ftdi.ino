void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
}

void loop()
{
   String  str = Serial.readString();
  for(int i=0;i<5;++i) {
    Serial.println(str);
  }

}


