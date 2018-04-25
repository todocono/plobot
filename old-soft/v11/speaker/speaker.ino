


const int rst_wifi = 0;   //wifi
const int opt_A = 1;    //opt
const int opt_B = 2;    //opt
const int spk_A = 3;    //spk
const int spk_B = 4;    //spk
const int miso = 5;       //miso

const int motor_l_A = 12;
const int motor_l_B = 13;
const int motor_r_A = 14;
const int motor_r_B = 15;

const int outputPin = 21;    //leds
const int pwr_off   = 26;    //pwr off

void chip_tone(unsigned long for_millis, unsigned long period_micros, int volume) {
  for (const unsigned long sm = millis(); (millis() - sm) < for_millis;) {
    digitalWrite(spk_B, HIGH);
    OCR0A = 0;
    delayMicroseconds(period_micros / 6);

    OCR0A = volume;
    delayMicroseconds(period_micros / 6);

    OCR0A = 0;
    delayMicroseconds(period_micros / 6);

    OCR0A = 0;
    delayMicroseconds(period_micros / 6);

    digitalWrite(spk_B, LOW);
    OCR0A = volume;
    delayMicroseconds(period_micros / 6);

    OCR0A = 0;
    delayMicroseconds(period_micros / 6);
  }
  OCR0A = volume;
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


  pinMode(motor_l_A, OUTPUT);
  pinMode(motor_l_B, OUTPUT);
  pinMode(motor_r_A, OUTPUT);
  pinMode(motor_r_B, OUTPUT);
  pinMode(spk_A, OUTPUT);
  pinMode(spk_B, OUTPUT);
  digitalWrite(spk_A, LOW);
  digitalWrite(spk_B, LOW);

//  // Off: don't burn speaker
//  OCR0A = 0;
//  TCCR0B = (TCCR0B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
//
//  // 62.5Khz fast PWM
//
//  // Use internal clock (datasheet p.160)
//  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
//
//  // Set fast PWM mode  (p.157)
//  TCCR0A |= _BV(WGM21) | _BV(WGM20);
//  TCCR0B &= ~_BV(WGM22);
//
//  // Do non-inverting PWM on pin OC2A (p.155)
//  // On the Arduino this is pin 11.
//  TCCR0A = (TCCR0A | _BV(COM0A1)) & ~_BV(COM0A0);
//  TCCR0A &= ~(_BV(COM2B1) | _BV(COM2B0));
//  TCCR0B = (TCCR0B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

}

void loop() {

//  while (true) {
//    Serial.println("Playing...");
//    go_jingle();
//    Serial.println("Waiting...");
//    OCR0A = 0;
//    delay(500);
//  }
  
 tone (spk_B,440,500);
   delay(1000);
 tone (spk_A,440,500);
  delay(1000);
}
