
const int dled = 30;
const int mic = 31;

int avg_val = 0;

void setup() {
  pinMode(mic, INPUT);
  pinMode(dled, OUTPUT);
  
  long avg_val_accum = 0L;
  for(int i=0;i<10;++i) {
    avg_val_accum += analogRead(mic);
    delay(1);
  }
  avg_val = avg_val_accum / 10L;
}

void loop() {
  while(true) {
    const int val = analogRead(mic) - avg_val;
    digitalWrite(dled, HIGH);
    delay(abs(val) / 10);
    digitalWrite(dled, LOW);
    delay(10);
  }
}
