
const int dled = 30;
const int encoder_pins[2] = {28, 27};

void setup() {
  pinMode(dled, OUTPUT);
  pinMode(encoder_pins[0], INPUT);
  pinMode(encoder_pins[1], INPUT);
}

void loop() {
  while(true) {
    int x = 2;
    if(digitalRead(encoder_pins[0]) == HIGH)
      --x;
    if(digitalRead(encoder_pins[1]) == HIGH)
      --x;
    digitalWrite(dled, LOW);
    delayMicroseconds(100+100 * x);
    digitalWrite(dled, HIGH);
    delayMicroseconds(100);
    
  }
}
