
const int motor_l_fwd = 14;
const int motor_l_back = 15;

const int motor_r_en = 13;

const int motor_r_fwd = PB3;
const int motor_r_back = PB5;
const int rfid_pulse = PB4;

const int spk_en_pin = 12;

const int motor_r_pulse = 30;
const int motor_l_pulse = 29;

const int antenna_off_pin = PB4;

const unsigned long sMinimumTimeBetweenPulses = 200;

// Try to filter any 32khz pwm
const unsigned long sMinimumPulseLength = 32;


void setup() {
  pinMode(motor_l_fwd, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_r_fwd, OUTPUT);
  pinMode(motor_r_back, OUTPUT);
  pinMode(motor_l_back, OUTPUT);
  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);

  pinMode(antenna_off_pin, OUTPUT);

  pinMode(spk_en_pin, OUTPUT);

  pinMode(rfid_pulse, OUTPUT);

  Serial.begin(9600);

  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
  TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz
}

class PulseCounter
{ public:
    PulseCounter(int pin)
      : pin_(pin),
        last_pulse_start_(0),
        pulses_(0), last_state_(digitalRead(pin))
    {
      //pinMode(rfid_pulse, OUTPUT);
    }

    void count() {
      const int state = digitalRead(pin_);
      if (pin_ == motor_r_pulse)
        digitalWrite(rfid_pulse, state);

      if (state != last_state_) {
        ++pulses_;
        last_state_ = state;
      }

    }

    unsigned pulses()const {
      return pulses_;
    }
  private:
    const int pin_;

    int last_state_;
    unsigned pulses_;
    unsigned long last_pulse_start_;
    unsigned long this_pulse_start_;
};

const int mtr_r_speed = 255, mtr_l_speed = 255;


void stop() {
  digitalWrite(motor_r_en, HIGH);
  digitalWrite(motor_r_fwd, HIGH);
  digitalWrite(motor_r_back, HIGH);
  digitalWrite(motor_l_fwd, HIGH);
  digitalWrite(motor_l_back, HIGH);
}

void coast() {
  digitalWrite(motor_r_en, LOW);
  digitalWrite(motor_r_fwd, LOW);
  digitalWrite(motor_r_back, LOW);
  digitalWrite(motor_l_fwd, LOW);
  digitalWrite(motor_l_back, LOW);
}

void loop() {
  coast();
  PulseCounter count_left(motor_l_pulse);
  PulseCounter count_right(motor_r_pulse);
  long left_stop_t = 0, right_stop_t = 0;
  const int pulses = 50;

  digitalWrite(motor_r_fwd, HIGH);
  analogWrite(motor_r_en, 200);
  analogWrite(motor_l_fwd, 200);
  // analogWrite(motor_l_back, 255);

  while (count_left.pulses() < pulses || count_right.pulses() < pulses) {
    count_left.count();
    count_right.count();
    Serial.print( count_left.pulses(), DEC );
    Serial.print ( "from left and ");
    Serial.print( count_right.pulses(), DEC );
    Serial.println ( "from right  ");    
  }

  //delay(1000);
  stop();

  delay(2000);
  /*
  Serial.print("Left pulses: ");
  Serial.println(count_left.pulses());
  Serial.print("Right pulses: ");
  Serial.println(count_right.pulses());
  Serial.print("Offset ");
  Serial.println(int(count_left.pulses()) - int(count_right.pulses()));
  */


}

