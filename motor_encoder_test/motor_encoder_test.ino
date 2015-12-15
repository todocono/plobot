
const int sEncoderPins[] = {0, 1};
const int sEncoderCount = sizeof(sEncoderPins) / sizeof(*sEncoderPins);

const int sMotorCount = 2;
const int sMotorEnable[sMotorCount] = {12, 13};
const int sMotorPositive[sMotorCount]  = {19, 21};
const int sMotorNegative[sMotorCount] =  {20, 22};

void setup() {
  for(int i=0;i<sEncoderCount;++i) {
    pinMode(sEncoderPins[i], INPUT);
    digitalWrite(sEncoderPins[i], HIGH);
  }
  for(int i=0;i<2;++i) {
    pinMode(sMotorEnable[i], OUTPUT);
    pinMode(sMotorPositive[i], OUTPUT);
    pinMode(sMotorNegative[i], OUTPUT);
  }
  
  Serial.begin(9600);
}

void stop_all_motors()
{
  for(int i=0;i<2;++i) {
    digitalWrite(sMotorEnable[i], LOW);
    digitalWrite(sMotorPositive[i], LOW);
    digitalWrite(sMotorNegative[i], LOW);
  }
  delayMicroseconds(100);
}

void run_motor(int m, bool dir) 
{
  digitalWrite(sMotorPositive[m], LOW);
  digitalWrite(sMotorNegative[m], LOW);
  delayMicroseconds(100);
  digitalWrite(sMotorEnable[m], HIGH);
  digitalWrite(sMotorPositive[m], dir ? HIGH : LOW);
  digitalWrite(sMotorNegative[m], dir ? LOW : HIGH);
  delayMicroseconds(100);
}

void stop_motor(int m) {
  digitalWrite(sMotorEnable[m], LOW);
  delayMicroseconds(100);
}

void count_encoders_for(int ms)
{
  int last_states[sEncoderCount];
  int counts[sEncoderCount];
  for(int i=0;i<sEncoderCount;++i) {
    last_states[i] = digitalRead(sEncoderPins[i]);
    counts[i] = 0;
  }
  for(unsigned long start_m=millis();(millis() - start_m) < ms;) {
    for(int i=0;i<sEncoderCount;++i) {
      int state = digitalRead(sEncoderPins[i]);
      if(state != last_states[i]) {
        ++counts[i];
        last_states[i] = state;
      }
    }
  }
  Serial.print("Counts:");
  for(int i=0;i<sEncoderCount;++i) {
    Serial.print(counts[i]);
    Serial.println(" ");
  }
  Serial.println();
}

void loop() {
  for(int m0=0;m0<2;++m0) {
    for(int m1=0;m1<2;++m1) {
      Serial.print("Beginning test: ");
      Serial.print(m0);
      Serial.print(" ");
      Serial.print(m1);
      Serial.println();
      if(m0)
        run_motor(0, true);
      else
        stop_motor(0);
      if(m1)
        run_motor(1, true);
      else 
        stop_motor(1);
      count_encoders_for(2000);
      stop_all_motors();
      delay(1000);
    }
  }
}

