
const int dled = 30;
const int trigPin = 19;
const int echoPin = 20;


const int pin_spk_en = 15;
const int pin_spk_dir = 23;


const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

void setup() {
  pinMode(dled, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);
  digitalWrite(pin_spk_en, LOW);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  Serial.begin(57600);
  
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 10000L);
  distance = (duration/2) / 29.1;
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(50);
}
