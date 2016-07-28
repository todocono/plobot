
const int pin_spk_en = 15;
const int pin_spk_dir = 23;

const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

void chip_tone(unsigned long for_millis, unsigned long period_micros, int volume) {
  for(const unsigned long sm=millis();(millis() - sm) < for_millis;) {
    digitalWrite(pin_spk_dir, HIGH);
    OCR2A = 0;
    delayMicroseconds(period_micros / 6);

    OCR2A = volume;
    delayMicroseconds(period_micros / 6);

    OCR2A = 0;
    delayMicroseconds(period_micros / 6);

    OCR2A = 0;
    delayMicroseconds(period_micros / 6);

    digitalWrite(pin_spk_dir, LOW);
    OCR2A = volume;
    delayMicroseconds(period_micros / 6);

    OCR2A = 0;
    delayMicroseconds(period_micros / 6);
  }
  OCR2A = volume;
}

void go_jingle() {
  chip_tone(300, 1250, 100);
  delay(200);
  chip_tone(300, 1250, 100);
  delay(200);
  chip_tone(300, 800, 100);
}

void setup() {
  Serial.begin(9600);
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);

  // Off: don't burn speaker
  OCR2A = 0;
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // 62.5Khz fast PWM

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
}

void loop() {

  while(true) {
    Serial.println("Playing...");
    go_jingle();
    Serial.println("Waiting...");
    OCR2A = 0;
    delay(500);
  }
}
