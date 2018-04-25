/*
const int mtr_l_fwd = 2;
const int mtr_l_en = 13;
const int mtr_l_back = 14;
*/


const int mtr_l_fwd = 30;
const int mtr_l_en = 12;
const int mtr_l_back = 29;


void setup() {
  pinMode(mtr_l_fwd, OUTPUT);
  pinMode(mtr_l_en, OUTPUT);
  pinMode(mtr_l_back, OUTPUT);
}

void loop() {
  digitalWrite(mtr_l_en, LOW);
  delay(500);
  digitalWrite(mtr_l_fwd, HIGH);
  digitalWrite(mtr_l_back, LOW);
  digitalWrite(mtr_l_en, HIGH);
  delay(1000);

  digitalWrite(mtr_l_en, LOW);
  delay(500);
  digitalWrite(mtr_l_fwd, LOW);
  digitalWrite(mtr_l_back, HIGH);
  digitalWrite(mtr_l_en, HIGH);
  delay(1000);
}
