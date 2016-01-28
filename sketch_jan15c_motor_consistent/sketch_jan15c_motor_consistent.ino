
const int motor_l_fwd = 14;
const int motor_l_back = 15;

const int motor_r_en = 13;

const int motor_r_fwd = PB3;
const int motor_r_back = PB5;


const int spk_en_pin = 12;

const int motor_r_pulse = 27;
const int motor_l_pulse = 28;

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
  
  pinMode(antenna_off_pin, OUTPUT);
  
  pinMode(spk_en_pin, OUTPUT);
  
  
  Serial.begin(9600);

  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
  TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz
}

class PulseCounter
{public:
   PulseCounter(int pin)
     : pin_(pin), 
       last_pulse_start_(0),
       pulses_(0), last_state_(digitalRead(pin))
   {
     //pinMode(rfid_pulse, OUTPUT);
   }
   
   void count() {
     const int state = digitalRead(pin_);
     
     if(state == LOW && last_state_ == HIGH) {
       // Pulse start
       const unsigned long now = micros();
       if((now - last_pulse_start_) > sMinimumTimeBetweenPulses) {
         last_state_ = LOW;
         this_pulse_start_ = now;
         /*
         if(pin_ == motor_r_pulse)
           digitalWrite(rfid_pulse, HIGH);
           */
       }
     } else if(state == HIGH && last_state_ == LOW) {
       // Pulse end
       last_state_ = HIGH;

       const unsigned long now = micros();
       if((now - this_pulse_start_) > sMinimumPulseLength) {
         ++pulses_;
         last_pulse_start_ = this_pulse_start_;
       }
/*
       if(pin_ == motor_r_pulse)
         digitalWrite(rfid_pulse, LOW);*/
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

const int mtr_r_speed = 255, mtr_l_speed = 240;


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
  /*
  digitalWrite(antenna_off_pin, HIGH);
        do_move(-1,-1,300);    
        return;
      s  */
    
  static int t_idx = 0;
  const boolean dir = ((t_idx++) % 2) == 0;
  
  coast();
  const unsigned long sm = micros();
  PulseCounter count_left(motor_l_pulse);
  PulseCounter count_right(motor_r_pulse);

  long left_stop_t = 0, right_stop_t = 0;
  const int pulses = dir ? 300 : 140;
  
  if(dir)
    return;

  if(dir)
    digitalWrite(motor_r_fwd, HIGH);
  else
    digitalWrite(motor_r_back, HIGH);
  
  const int avg_speed = (mtr_r_speed + mtr_l_speed) / 2;

  analogWrite(motor_r_en, avg_speed);  
  analogWrite(motor_l_fwd, avg_speed);
  
  // Noise at signal start
 //delay(100);
 delay(50);

  Serial.println("-----");
  while(count_left.pulses() < pulses || count_right.pulses() < pulses) {
    count_left.count();
    count_right.count();
    /*
    if(count_left.pulses() == pulses && left_stop_t == 0) {
      analogWrite(motor_l_fwd, 0);
      left_stop_t = micros();
    }
    
    if(count_right.pulses() == pulses && right_stop_t == 0) {
      analogWrite(motor_r_en, 0);  
      right_stop_t = micros();
    }
    */
    
    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    //if(max(iclp, icrp) > 30)
    {
      if(abs(int(iclp) - int(icrp)) > 1) {
        /*
        static int pi = 0;
        if((++pi) % 100 == 0) {
          Serial.print(iclp);
          Serial.print(" ");
          Serial.println(icrp);
        }*/
        
        if(iclp > icrp) {
          analogWrite(motor_r_en, mtr_r_speed);  
          analogWrite(motor_l_fwd, 0); // 0
        } else {
          analogWrite(motor_r_en, 0);  // 0
          analogWrite(motor_l_fwd, mtr_l_speed);
        }
        
      } else {
        analogWrite(motor_r_en, mtr_r_speed);  
        analogWrite(motor_l_fwd, mtr_l_speed);
      }
    }
  }
  stop();
  
  Serial.print("left off millis ");
  Serial.println(left_stop_t / float(1000));
  Serial.print("right off millis ");
  Serial.println(right_stop_t / float(1000));
  Serial.print("offset ");
  Serial.println((left_stop_t - right_stop_t) / float (1000));
  
  /*
  Serial.print("Left pulses: ");
  Serial.println(count_left.pulses());
  Serial.print("Right pulses: ");
  Serial.println(count_right.pulses());
  Serial.print("Offset ");
  Serial.println(int(count_left.pulses()) - int(count_right.pulses()));
  */
  
  delay(500);  
}

