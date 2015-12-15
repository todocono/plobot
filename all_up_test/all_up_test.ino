
const int sEncoderPins[] = {0, 1};
const int sEncoderCount = sizeof(sEncoderPins) / sizeof(*sEncoderPins);

const int sMotorCount = 2;
const int sMotorEnable[sMotorCount] = {12, 13};
const int sMotorPositive[sMotorCount]  = {19, 21};
const int sMotorNegative[sMotorCount] =  {20, 22};
const int sSpeakerPin = 15;

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
  pinMode(sSpeakerPin, OUTPUT);
  
  delay(1000);
  
  // Motor Test
  run_motor(0, false);
  run_motor(1, false);
  delay(1000);
  
  
  run_motor(0, true);
  run_motor(1, true);
  delay(1000);

  stop_all_motors();
}

void loop() {
  while(true) {
    digitalWrite(sSpeakerPin, LOW);
    delayMicroseconds(1500);
    digitalWrite(sSpeakerPin, HIGH);
    delayMicroseconds(1500);
  }
}

