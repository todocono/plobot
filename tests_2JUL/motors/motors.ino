
const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

const int pin_spk_en = 15;
const int pin_spk_dir = 23;


void setup() {
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Gogogo");
}

void loop() {
  digitalWrite(motor_l_dir, LOW);
  analogWrite(motor_l_en, 128);
  delay(750);
  analogWrite(motor_l_en, 0);
  delay(300);
  digitalWrite(motor_l_dir, HIGH);
  analogWrite(motor_l_en, 128);
  delay(750);
  analogWrite(motor_l_en, 0);
  delay(300);
  
  digitalWrite(motor_r_dir, HIGH);
  analogWrite(motor_r_en, 128);
  delay(750);
  analogWrite(motor_r_en, 0);
  delay(300);
  digitalWrite(motor_r_dir, LOW);
  analogWrite(motor_r_en, 128);
  delay(750);
  analogWrite(motor_r_en, 0);
  delay(300);
}
