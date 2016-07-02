
const int dled = 30;
const int ultra_power = 22;
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
  pinMode(ultra_power, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(ultra_power, HIGH);

  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  Serial.begin(9600);
  
}

void loop() {
  digitalWrite(ultra_power, HIGH);
  delay(500);
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
 // digitalWrite(ultra_power, LOW);
  delay(500);
}
