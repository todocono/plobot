
const int rst_wifi = 0;

void setup()
{
  pinMode(rst_wifi, OUTPUT);

  digitalWrite(rst_wifi, HIGH);
  
  Serial.begin(9600);
  Serial1.begin(115200); // 115200
  
  Serial.println("-- start --");
}
void loop()
{
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
    while (Serial.available()) {
      const char b = Serial.read();
      Serial.write(b);
        Serial1.write(b);
    }
    
  if((millis() % 2000L) > 1950L) {
    Serial.println("--- resetting ---");
    digitalWrite(rst_wifi, LOW);
    delay(100);
    digitalWrite(rst_wifi, HIGH);
  }

}


