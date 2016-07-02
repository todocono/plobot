
const int pin_spk_en = 15;
const int pin_spk_dir = 23;


const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

void setup() {
  Serial.begin(9600);
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  analogWrite(pin_spk_en, 150);
}

void loop() {

  while(true) {
    Serial.println("Playing...");
    for(int level=0;level<4;++level) { 
      for(unsigned long m=millis();millis() < (m + 100);) {
        unsigned long millis_per_cycle = 1 + level + (m%100) / 50;
        analogWrite(pin_spk_en, 50);
  
        digitalWrite(pin_spk_dir, HIGH);
        delay(millis_per_cycle);
        digitalWrite(pin_spk_dir, LOW);
        delay(millis_per_cycle);
      }
    }
    Serial.println("Waiting...");
    analogWrite(pin_spk_en, 0);
    delay(500);
  }
}
