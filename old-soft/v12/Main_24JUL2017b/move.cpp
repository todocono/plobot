
#include "move.h"
#include <L3G4200D.h>
#include <PID_v1.h>
#include <Wire.h>

const int motor_l_pulse = 27;
const int motor_r_pulse = 28;

const int motor_l_a = 12;
const int motor_l_b = 13;

const int motor_r_a = 14;
const int motor_r_b = 15;



class PulseCounter
{ public:
    PulseCounter(int pin)
      : pin_(pin),
        pulses_(0), last_state_(digitalRead(pin))
    {
    }

    void count() {
      const int state = digitalRead(pin_);

      if (state == LOW && last_state_ == HIGH) {
        // Pulse start
        last_state_ = LOW;
      } else if (state == HIGH && last_state_ == LOW) {
        // Pulse end
        last_state_ = HIGH;

        ++pulses_;
      }
    }

    int pulses()const {
      return pulses_;
    }
  private:
    int pin_;

    int last_state_;
    int pulses_;
};

PulseCounter count_left(motor_l_pulse);
PulseCounter count_right(motor_r_pulse);

void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

ISR (PCINT0_vect)
{
  noInterrupts();
  count_left.count();
  count_right.count();
  interrupts();
}

void init_movement() {
  // do we need to adjust TCCR1A as well?
  // TCCR1B = TCCR1B & 0b11011000 | 0x01; //noise cancelling, rising edge, no prescaling and ?? may be?? fast pwm

  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);

  pinMode(motor_l_a, OUTPUT);
  pinMode(motor_l_b, OUTPUT);
  pinMode(motor_r_a, OUTPUT);
  pinMode(motor_r_b, OUTPUT);

  pciSetup(motor_l_pulse);
  pciSetup(motor_r_pulse);

  Wire.begin();

  // TODO: Hack: First calibration is somehow wrong
  {
    L3G4200D gyroscope;
    while (!gyroscope.begin(L3G4200D_SCALE_250DPS, L3G4200D_DATARATE_400HZ_50))     // Set scale 250 dps and 400HZ Output data rate (cut-off 50)
    {
      Serial.println("Could not find the L3G4200D sensor, check wiring!");
      return;
    }
    gyroscope.calibrate(100);
  }
}

void turn(int degs)
{
  L3G4200D gyroscope;
  while (!gyroscope.begin(L3G4200D_SCALE_250DPS, L3G4200D_DATARATE_400HZ_50))  // Set scale 250 dps and 400HZ Output data rate (cut-off 50)
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    return;
  }
  gyroscope.calibrate(100);

  count_left = PulseCounter(motor_l_pulse);
  count_right = PulseCounter(motor_r_pulse);

  double Setpoint = 0, Input = 0, Output = 0;
  const double scale_factor = 7000000.0f;
  PID myPID(&Input, &Output, &Setpoint, 9.5 / scale_factor, 4 / scale_factor, 3.5 / scale_factor, DIRECT);
  myPID.SetMode(AUTOMATIC);
  // Max should be <255 for bootstrap
  myPID.SetOutputLimits(-70, 70);

  long z_total = 0;
  Input = 0;
  const long target_z = long(float(degs) * (90000000.0f / 90.0f));
  unsigned long last_off_target = millis();
  unsigned long last_read_sensor = micros();
  const unsigned long started_turn = millis();

  Setpoint = target_z;
  // Grace period to start moving
  long last_not_moving = millis() + 50;
  int min_power = 100;

  int direction;

  while ((millis() - last_off_target) < 100L && (millis() - started_turn) < 2000L)
  {
    const int max_pulses = max(count_left.pulses(), count_right.pulses());
    const long now_millis = millis();
    const long bump_min_speed_frequency = 10;
    if (max_pulses < 5 && (now_millis - last_not_moving) > bump_min_speed_frequency) {
      last_not_moving += bump_min_speed_frequency;
      min_power += 5;
    }

    unsigned long amount_off_target = abs(z_total - target_z);
    if (amount_off_target > 2506516L) {
      last_off_target = millis();
    }

    const unsigned long read_sensor_time = micros();
    Vector norm = gyroscope.readNormalize();
    const long time_since_read = (read_sensor_time - last_read_sensor);
    last_read_sensor = read_sensor_time;

    long yaw = (long)norm.ZAxis;
    z_total += yaw * time_since_read;

    Input = double(z_total);
    myPID.Compute();

    if (Output > 0) {
      direction = 0;
    }
    else {
      direction = 1;
    }

    int mtr_pwr = min_power + abs(Output);

    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    const int encoder_diff = int(iclp) - int(icrp);

    if (abs(encoder_diff) > 5) {
      if (iclp > icrp) {
        motorRight( 0, direction);
        motorLeft( mtr_pwr, direction);
      }
      else {
        motorRight( mtr_pwr, direction);
        motorLeft( 0, direction);
      }
    }
    else {
      motorRight( mtr_pwr, direction);
      motorLeft( mtr_pwr, direction);
    }
  }
  motorLeft( 0, direction);
  motorLeft( 0, direction);
}

void move_straight(int pulses)
{
  count_left = PulseCounter(motor_l_pulse);
  count_right = PulseCounter(motor_r_pulse);

  const unsigned long sm = micros();
  int direction;
  if (pulses >= 0) {
    direction = 0;
  } else {
    direction = 1;
  }

  pulses = abs(pulses);

  unsigned long last_off_target = millis();
  const unsigned long started_move = millis();

  // Max should be <255 for bootstrap
  int min_power = 120, max_power = 180;
  // Grace period to start moving
  long last_not_moving = millis() + 100;

  while ((millis() - last_off_target) < 500 && (millis() - started_move) < 2000L) {
    const long now_millis = millis();
    const int max_pulses = max(count_left.pulses(), count_right.pulses());
    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    const boolean mismatch = abs(int(iclp) - int(icrp)) > 5;
    const boolean arrived = max_pulses >= pulses;
    if (!arrived || mismatch) {
      last_off_target = millis();
    }
    const int avg_pulses = (count_left.pulses() + count_right.pulses()) / 2;
    const int triangle = (avg_pulses > pulses / 2) ? (pulses - 16 * avg_pulses) : 4 * avg_pulses;
    const int mtr_speed = max(min_power, min(max_power, min_power + triangle));

    const long bump_min_speed_frequency = 20;
    if (max_pulses < 5 && (now_millis - last_not_moving) > bump_min_speed_frequency) {
      last_not_moving += bump_min_speed_frequency;
      min_power += 5;
      max_power += 5;
    }

    if (!arrived) {
      if (mismatch) {
        if (iclp > icrp) {
          motorRight( 0, direction);
          motorLeft( mtr_speed, direction);
        } else {
          motorRight( mtr_speed, direction);
          motorLeft( 0, direction);
        }
      } else {
        motorRight( mtr_speed, direction);
        motorLeft( mtr_speed, direction);
      }
    } else {
      if (abs(int(iclp) - int(icrp)) < 15) {
        motorRight( mtr_speed, direction);
        motorLeft( mtr_speed, direction);
      } else {
        if (iclp > icrp) {
          motorRight( 0, direction);
          motorLeft( min_power, direction);
        } else {
          motorRight( min_power, direction);
          motorLeft( 0, direction);
        }
      }
    }
  }
  motorRight( 0, direction);
  motorLeft( 0, direction);
}

void motorLeft( int speed, int direction) {
  if (!speed) {
    analogWrite(motor_l_a, LOW);
    analogWrite(motor_l_b, LOW);
  } else {
    if (!direction) {
      analogWrite(motor_l_a, LOW);
      analogWrite(motor_l_b, speed);
    } else {
      analogWrite(motor_l_a, speed);
      analogWrite(motor_l_b, LOW);
    }
  }
}

void motorRight( int speed, int direction) {
  if (!speed) {
    analogWrite(motor_r_a, LOW);
    analogWrite(motor_r_b, LOW);
  } else {
    if (!direction) {
      analogWrite(motor_r_a, LOW);
      analogWrite(motor_r_b, speed);
    } else {
      analogWrite(motor_r_a, speed);
      analogWrite(motor_r_b, LOW);
    }
  }
}
