/*
 Plobot OS

 This code uses bitlash as an interpreter (thanks BillRoy http://bitlash.net)
 It's base is to execute commands that are stored in a buffer.
 Input of commands might be RFID for young kids or through serial protocol over WIFI

 The circuit:
 * LED attached from digital pin 13, 12 and 9  //note that on current ATmega644 there is no PWM output.
 * Anode to VCC (resistors of 330ohms)

 * RFID attached to Serial

 Created  28 March 2015
 Modified 02 April 2015\
 Last Update 04 July 2015
 By Rodolfo Cossovich [rudi]


 http://plobotics.org

 This example code is in the public domain.

 */
#define MOTOR
#define ENCODERS 1
#define MUSIC

#include <SM.h>
#include "bitlash.h"


#ifdef MOTOR
#include <DCMotor.h>

int M0EN = 12;
int M01  = 19;
int M02 =  20;

int M1EN =  13;
int M11 =  21;
int M12 =  22;

DCMotor motor0(M0EN, M01, M02);
DCMotor motor1(M1EN, M11, M12);
#endif

#define RED_LED    3//13
#define BLUE_LED   4//12
#define GREEN_LED  14//9

#define RFID_PACK  10

#define VCC_LED  2

#ifdef ENCODERS
#define ENCODER_T 250
#define speedMotors 80.0
#define LEVEL_ENCODER 255

#define INTERRUPTS 0

#define ENCODER1 1
#define ENCODER2 0

// ideally these would be flags and with assembler we increment the counter that is refreshed in main loop...
volatile unsigned long int encoderCounter1 = 0;   // counter for the times the encoder changed state
volatile byte encoderState1 = 0;         // current state of encoder as read by interrrupt service
volatile byte lastEncoderState1 = 0;     // previous state of encoder
volatile unsigned long int encoderCounter2 = 0;   // counter for the times the encoder changed state
volatile byte encoderState2 = 0;
volatile byte lastEncoderState2 = 0;

volatile int stepsA = 0;
volatile int stepsB = 0;
#endif

#ifdef MUSIC
#define SPEAKER 15

#define OCTAVE_OFFSET 0

// These values can also be found as constants in the Tone library (Tone.h)
int notes[] = {
  0,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};

//char *song = "The Simpsons:d=4,o=5,b=160:c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g,8p,8p,8f#,8f#,8f#,8g,a#.,8c6,8c6,8c6,c6";
//char *song = "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
//char *song = "TakeOnMe:d=4,o=4,b=160:8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5,8f#5,8e5,8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5";
//char *song = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";
//char *song = "Muppets:d=4,o=5,b=250:c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,8a,8p,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,8e,8p,8e,g,2p,c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,a,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,d,8d,c";
//char *song = "Xfiles:d=4,o=5,b=125:e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,g6,f#6,e6,d6,e6,2b.,1p,g6,f#6,e6,d6,f#6,2b.,1p,e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,e6,2b.";
//char *song = "Looney:d=4,o=5,b=140:32p,c6,8f6,8e6,8d6,8c6,a.,8c6,8f6,8e6,8d6,8d#6,e.6,8e6,8e6,8c6,8d6,8c6,8e6,8c6,8d6,8a,8c6,8g,8a#,8a,8f";
//char *song = "20thCenFox:d=16,o=5,b=140:b,8p,b,b,2b,p,c6,32p,b,32p,c6,32p,b,32p,c6,32p,b,8p,b,b,b,32p,b,32p,b,32p,b,32p,b,32p,b,32p,b,32p,g#,32p,a,32p,b,8p,b,b,2b,4p,8e,8g#,8b,1c#6,8f#,8a,8c#6,1e6,8a,8c#6,8e6,1e6,8b,8g#,8a,2b";
//char *song = "Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6";
//char *song = "MASH:d=8,o=5,b=140:4a,4g,f#,g,p,f#,p,g,p,f#,p,2e.,p,f#,e,4f#,e,f#,p,e,p,4d.,p,f#,4e,d,e,p,d,p,e,p,d,p,2c#.,p,d,c#,4d,c#,d,p,e,p,4f#,p,a,p,4b,a,b,p,a,p,b,p,2a.,4p,a,b,a,4b,a,b,p,2a.,a,4f#,a,b,p,d6,p,4e.6,d6,b,p,a,p,2b";
//char *song = "StarWars:d=4,o=5,b=45:32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#.6,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#6";
//char *song = "GoodBad:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#";
//char *song = "TopGun:d=4,o=4,b=31:32p,16c#,16g#,16g#,32f#,32f,32f#,32f,16d#,16d#,32c#,32d#,16f,32d#,32f,16f#,32f,32c#,16f,d#,16c#,16g#,16g#,32f#,32f,32f#,32f,16d#,16d#,32c#,32d#,16f,32d#,32f,16f#,32f,32c#,g#";
//char *song = "A-Team:d=8,o=5,b=125:4d#6,a#,2d#6,16p,g#,4a#,4d#.,p,16g,16a#,d#6,a#,f6,2d#6,16p,c#.6,16c6,16a#,g#.,2a#";
//char *song = "Flinstones:d=4,o=5,b=40:32p,16f6,16a#,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,d6,16f6,16a#.,16a#6,32g6,16f6,16a#.,32f6,32f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,a#,16a6,16d.6,16a#6,32a6,32a6,32g6,32f#6,32a6,8g6,16g6,16c.6,32a6,32a6,32g6,32g6,32f6,32e6,32g6,8f6,16f6,16a#.,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#6,16c7,8a#.6";
//char *song = "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#";
char *song = "Gadget:d=16,o=5,b=50:32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,32d#,32f,32f#,32g#,a#,d#6,4d6,32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,8d#";
//char *song = "Smurfs:d=32,o=5,b=200:4c#6,16p,4f#6,p,16c#6,p,8d#6,p,8b,p,4g#,16p,4c#6,p,16a#,p,8f#,p,8a#,p,4g#,4p,g#,p,a#,p,b,p,c6,p,4c#6,16p,4f#6,p,16c#6,p,8d#6,p,8b,p,4g#,16p,4c#6,p,16a#,p,8b,p,8f,p,4f#";
//char *song = "MahnaMahna:d=16,o=6,b=125:c#,c.,b5,8a#.5,8f.,4g#,a#,g.,4d#,8p,c#,c.,b5,8a#.5,8f.,g#.,8a#.,4g,8p,c#,c.,b5,8a#.5,8f.,4g#,f,g.,8d#.,f,g.,8d#.,f,8g,8d#.,f,8g,d#,8c,a#5,8d#.,8d#.,4d#,8d#.";
//char *song = "LeisureSuit:d=16,o=6,b=56:f.5,f#.5,g.5,g#5,32a#5,f5,g#.5,a#.5,32f5,g#5,32a#5,g#5,8c#.,a#5,32c#,a5,a#.5,c#.,32a5,a#5,32c#,d#,8e,c#.,f.,f.,f.,f.,f,32e,d#,8d,a#.5,e,32f,e,32f,c#,d#.,c#";
//char *song = "MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";


#define isdigit(n) (n >= '0' && n <= '9')

#endif


unsigned long int tag[] = {
   //Multiple Cards from set YCIS - Shanghai
  //Special Cards
  29271,    // Tag 1   
  10820,    // Tag 2
  10850,    // Tag 3
  26791,    // Tag 4
  29285,    // Tag 5
  27526,    // Tag 6
  27540,    // Tag 7
  0x059398F,    // Tag 8    RUN
  0x08D5892,    // Tag 9    RESET
  123,// Tag 10
  //Music Cards
  0x0784611,    // Tag 11 BIRTHDAY
  0x0784619,    // Tag 12  MONEY
  0x0784615,    // Tag 13  POLICE
  0x0784617,    // Tag 14  BELL
  0x026425C,    // Tag 15  SOUND
  27731,    // Tag 16
  0x0785EF3,    // Tag 17
  0x08D5892,    // Tag 18          RESET
  0x0264258,    // Tag 19          REPEAT
  0x08D37E0,    //29104,    // Tag 20 PAUSE
  //Motor Cards
  0x0264256,    // Tag 21       FWD
  0x026425A,    // Tag 22       BKD
  0x026424B,    // Tag 23       RIGHT
  0x0264254,    // Tag 24       LEFT
  0x026426F,    // Tag 25        RED
  0x078555A,    // Tag 26        GREEN
  0x0785548,    // Tag 27        BLUE
  0x0785EFD,    // Tag 28
  1234,// 29291,    // Tag 29
  //SENSOR Cards
  0x0785552,    // Tag 30     WATCH
  0x0785554,    // Tag 31     SEARCH
  0x0785EFF,    // Tag 32     DRAW (untested)
  0x0785EE0,    // Tag 33     IF (untested)
  29291,    // Tag 34 1
  29291,    // Tag 35
  29291,    // Tag 36
};


int numTags = 35 + 1; // Number in that list

int validTag = 0;

// PowerSwitchTail unit pins and unit states:
//int numUnits = 8; // Two PowerSwitch Tails

//int unit[] = {2, 3}; // Pins 2 and 3
//int unitState[] = {LOW, LOW}; // Both start in the off position
long lastRead; // the time when we last read a tag
long timeOut = 300; // required time between reads

unsigned long int tempTAG = 0;
unsigned long int currentTAG = 0;
byte bytesRead = 0;

float in; // to make the LED smooth blink

int memoryExec[64];
int positionMemory = 0;
int iteratePositionMemory = 0;
#define totalMemory 64   //it was 16 but kids got bored easily


SM Simple(S3);//create simple statemachine

void setup() {
#ifdef MOTOR
  addBitlashFunction("mv", (bitlash_function) move_motors );
#endif
  addBitlashFunction("temp_blink", (bitlash_function) temp_blink );
  lastRead = millis(); // Initalize to the sketch's start time
  pinMode( GREEN_LED, OUTPUT);
  pinMode( BLUE_LED, OUTPUT);
  pinMode( RED_LED, OUTPUT);

  pinMode( VCC_LED, OUTPUT);
  digitalWrite( VCC_LED, HIGH);
  pinMode(SPEAKER, OUTPUT);

#if INTERRUPTS
  InitialiseInterrupt();
#endif

#ifdef ENCODERS

  pinMode(ENCODER1, INPUT);	   //
  digitalWrite(ENCODER1, HIGH);   //
  pinMode(ENCODER2, INPUT);	   //
  digitalWrite(ENCODER2, HIGH);   //
#endif



  Serial.begin(9600);     //Initialize serial
  Serial.println("Starting Plobot Engines...");

  initBitlash(9600);
  Serial1.begin(9600);    //RFID initiated

  // changeColor ( 255, 0, 0);
}


void loop() {
  if (Serial1.available()) readByte();
  EXEC(Simple);  // run statemachine
  runBitlash();  // keeps the latency, checks for new commands on Serial0

}

void count_encoders() {
  encoderState1 = digitalRead(ENCODER1);  // compare the encoder to its previous state
  if (encoderState1 != lastEncoderState1) { // if the current state is HIGH then the enocder changed pole from north to south
    encoderCounter1++;  // if the state has changed, increment the counter
    if  (encoderCounter1 >= stepsA)  motor0.setSpeed(0);
  }
  lastEncoderState1 = encoderState1;// save the current state as the last state,

  encoderState2 = digitalRead(ENCODER2);  // compare the encoder to its previous state
  if (encoderState2 != lastEncoderState2) { // if the current state is HIGH then the enocder changed pole from north to south
    encoderCounter2++;  // if the state has changed, increment the counter
    if  (encoderCounter2 >= stepsB ) motor1.setSpeed(0);
  }
  lastEncoderState2 = encoderState2;// save the current state as the last state,
}


State S1() {     //This state reads and records commands
  pulseLED();
  if ( currentTAG) {
    validTag = checkTags();
    currentTAG = 0;
    // Serial.println("it came back with: " +  String(validTag));
    if ( validTag ) {
      //changeColor ( 255, 0, 0);
      switch (validTag ) {
        case 1:
        case 2:
        case 3:    //break statements intentionally skipped
        case 4:    // all these cards will perform the following actions
        case 5:
        case 6:
        case 7:
        case 8:
          Serial.println("executing cards recorded");
          iteratePositionMemory = 0;

          changeColor(255, 0, 255);
          tone (SPEAKER, 660, 100);
          delay(500);
          changeColor(0, 0, 255);
          tone (SPEAKER, 660, 100);
          delay(500);
          changeColor(0, 255, 255);
          tone (SPEAKER, 770, 500);
          delay(700);
          changeColor(255, 255, 255);
          Simple.Set(S2);      // changes mode to code execution
          break;
        case 9:               // erases current program
          tone (SPEAKER, 440, 1000);
          Serial.print("erasing cards recorded.");
          for (int x = 0; x <= totalMemory; x++) {
            memoryExec [ x ] = 0;
            Serial.print(".");
          }
          Serial.println("!");
          positionMemory = 0;
          break;
        default:
          //Simple.Set(S3);    //it didn't recognize the card
          recordCard();
          break;

          // if nothing else matches, do the default
          // default is optional
      }
      validTag = 0;
    }
  }
}


State S2() { //This state executes the commands
  //tone (SPEAKER, 550, 100);
  if ( iteratePositionMemory < positionMemory ) {

    doCommand( (char*)readCommand( memoryExec[iteratePositionMemory] ));
    //doCommand("beep(22,660,1000);");
    //  char* response = (char*)readCommand( memoryExec[positionMemory] );

    Serial.print( "executing command with " + String( iteratePositionMemory ) );
    Serial.println( "that means: " + memoryExec[iteratePositionMemory]  );
    iteratePositionMemory++;
    // validTag = 0;
  }
  else {     // if it's the last command it changes state to S1
    Simple.Set(S3);//
  }
}


State S3() { //This state is ran after commands has been executed
  Simple.Set(S4);//wait for last command and change state to S4
}

State S4() { // This state runs after a startup or
  in = 0;
  doCommand( "g=0; b=0; r=0;" );
  // doCommand( "aw(10,g); aw(12,b);aw(13,r); \r\n" );
  delay(100);
  //changeColor ( 255, 255, 255);
  delay(500);
  Simple.Set(S1);//wait for toggle pin to go low and change state to S1
}


void readByte( void ) {
  // read the incoming byte:
  int incomingByte = Serial1.read();
  Serial.print(incomingByte, HEX );

  if (!bytesRead && !incomingByte) {  //I wait for the start byte with 0x00
    bytesRead = 1;
  }
  else {
    // say what you got:
    // Serial.print("I received: ");
    //Serial.println(incomingByte, HEX);
    if ( bytesRead++ > 0 ) {
      //add to the TAG id number, careful with the casting
      tempTAG = tempTAG << 8;
      tempTAG += incomingByte;
    }
    if ( bytesRead >= 4 ) {
      // here I could calculate checksum, with remaining to 0xFF
      currentTAG = tempTAG;
      bytesRead = 0;
      tempTAG = 0;
    }
  }
}

int checkTags() {
  Serial.print("checking: " );
  Serial.print(currentTAG, DEC);
  // iterate over the list of tags:
  for (int thisTag = 0; thisTag < numTags; thisTag++) {
    // if the current tag matches the tag you're on:
    //  Serial.println(" against:"+ String(tag[thisTag]) );
    if (currentTAG == tag[thisTag]) {
      // Only flip a switch if the tag has been away for a while
      if (lastRead + timeOut < millis()) {
        // unit number starts at 1, but list position starts at 0:
        Serial.println("TAG detected:  " + String(thisTag + 1));
        lastRead = millis(); // mark the last time you got a good tag
        return thisTag + 1;
      }
      else {
        Serial.println("too fast to re-read");
        return 0;
      }
    }
  }
  return 0;
}

char* readCommand( int commandId ) {
  // looks through a table of defined commands
  switch (commandId) {
    case 1: //break statement ommited on purpose
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      play_rtttl(song);
      return "";
      break;
    case 7:
      return "pinmode(a0,OUTPUT);a0=1;\r\n";       //A3
      break;
    case 8:
    case 9:
    case 10:
      return "";
      break;
    case 11:
      play_rtttl(song);
      return "";
      break;
    case 12:
      return  "beep(15,247,500);\r\n";    //B3
      break;
    case 13:
      return "beep(15,262,500);\r\n";      //C4
      break;
    case 14:
      return "beep(15,294,500);\r\n";      //D4
      break;
    case 15:
      return "beep(15,600,150);beep(15,330,250);beep(15,600,150);beep(15,330,250);beep(15,600,150);beep(15,330,250);beep(15,600,150);beep(15,330,250);\r\n";      //E4
      break;
    case 16:
      return  "beep(15,349,500);\r\n";     //F4
      break;
    case 17:
      return  "beep(15,392,500);\r\n";    //G4
      break;
    case 18:
      return  "";
      break;
    case 19:
      play_rtttl(song);
      return "";
      break;
    case 20:
      return "snooze(1000);\r\n";      //wait
      break;
    case 21:
      return "mv(2,-80,30,30); snooze(300);\r\n";    //Forward
      break;
    case 22:
      return "mv(2,80,30,30); snooze(300);\r\n";    //Back
      break;
    case 23:
      return "mv(3,80,35,35); snooze(300);\r\n";    //Right
      break;
    case 24:
      return "mv(4,80,35,35);  snooze(300);\r\n";    //Left
      break;
    case 25: //RED
      //return  "r=r+63; aw(3,r);\r\n;";    //
      //return color_flashes[sizeof(color_flashes)/sizeof(*color_flashes)];
      return "temp_blink();\r\n";
      break;
    case 26: //GREEN
      return  "g=g+63; aw(4,g); \r\n";
      break;
    case 27: //BLUE
      return  "b=b+63; aw(14,b); \r\n";    //
      break;
    default:
      return "print('no command entered')\r\n;";
      break;
  }
}

void recordCard ( void ) {
  analogWrite(GREEN_LED, 0);
  analogWrite(RED_LED, 255);
  analogWrite(BLUE_LED, 255);
  //CARD CODE RECORD   positionMemory++ < totalMemory
  if ( positionMemory < totalMemory ) {
    tone (SPEAKER, 550, 50);
    delay(100);
    Serial.print("recording card");
    memoryExec [positionMemory] = validTag;
    positionMemory++;
  }
  else {
    analogWrite(GREEN_LED, 255);
    analogWrite(RED_LED, 0);
    analogWrite(BLUE_LED, 255);
    tone (SPEAKER, 550, 50);
    delay(50);
    tone (SPEAKER, 660, 100);
    delay(50);
    tone (SPEAKER, 550, 50);
  }
  analogWrite(RED_LED, 0);
  analogWrite(BLUE_LED, 0);
  analogWrite(GREEN_LED, 0);
}

void temp_blink() {
  static const unsigned long colors[] = {
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0x00FF00FF,
    0x00FFFF00,
    0x00FFFFFF,
  };
  
  const int ci = random(sizeof(colors)/sizeof(*colors));
  
  for(int i=0;i<10;++i) {
    analogWrite(GREEN_LED, (colors[ci] & 0xFF) >> 0);
    analogWrite(RED_LED, (colors[ci] & 0xFF00) >> 8);
    analogWrite(BLUE_LED, (colors[ci] & 0xFF0000) >> 16);
    delay(100);

    analogWrite(GREEN_LED, 0);
    analogWrite(RED_LED, 0);
    analogWrite(BLUE_LED, 0);
    delay(50);
  }
  
}

#ifdef MOTOR
int move_motors(byte which, int fast, int in_stepsA, int in_stepsB) {
  stepsA = in_stepsA;
  stepsB = in_stepsB;
  
  
  if (!which) {
    motor0.brake();
    motor1.brake();
    return 1;
  }
  else {
    if (which == 0) {
      motor0.setSpeed(fast);
    }
    else if (which == 1) {
      motor1.setSpeed(fast);
    }
    else if (which == 2) {
      motor0.setSpeed(fast);
      motor1.setSpeed(fast);
    }
    else if (which == 3) {
      motor0.setSpeed(-fast);
      motor1.setSpeed(fast);
    }
    else if (which == 4) {
      motor0.setSpeed(fast);
      motor1.setSpeed(-fast);
    }

    long motorTimeout = millis() + ENCODER_T;
    encoderCounter1 = 0;
    encoderCounter2 = 0;
    Serial.print("Counting Left:");
    Serial.println(stepsA, DEC);
    Serial.print("Counting Right:");
    Serial.println(stepsB, DEC);
#ifdef ENCODERS
    // No serial prints allowed in this loop
    while (  (encoderCounter1 < stepsA) || (encoderCounter2 < stepsB )) {
#if !INTERRUPTS
     count_encoders();
#endif
      if  (encoderCounter1 > stepsA)  motor0.setSpeed(0);
      if  (encoderCounter2 > stepsB ) motor1.setSpeed(0);
      if (motorTimeout < millis()) {
        Serial.print("TIMEOUT -  check encoders: ");
        Serial.print("[");
        Serial.print(encoderCounter1);
        Serial.print(" ");
        Serial.print(stepsA);
        Serial.print("], [");
        Serial.print(encoderCounter2);
        Serial.print(" ");
        Serial.print(stepsB);
        Serial.println("]");
        motor0.setSpeed(0);    //if it was timeout
        motor1.setSpeed(0);
        return 0;
      }
    }  
#endif

    Serial.print( "Encoder 1 ran: ");
    Serial.println(encoderCounter1);
    Serial.print( "Encoder 2 ran: ");
    Serial.println(encoderCounter2);
    motor0.setSpeed(0);    //if it was nottimeout
    motor1.setSpeed(0);
    return 1;
    //  interrupts();
  }
  return 0;
}

numvar move_motors(void) {   // gets which DC motor,
  byte     which = getarg(1);
  int      fast = getarg(2);
  int arg_stepsA = getarg(3);
  int arg_stepsB = getarg(4);   // this might not be working
  return move_motors(which, fast, arg_stepsA, arg_stepsB);
}
#endif

void pulseLED(void) {
  analogWrite(RED_LED, 0);
  analogWrite(BLUE_LED, 0);
  in = in + 0.0005;
  if ( in < 6.283) {
    float out = sin(in) * 127.5 + 127.5;
    changeColor((int)out, (int)out, (int)out);
  }
  else {
    in = 0;
  }
}

void changeColor (int newValueR, int newValueG, int newValueB ) {
  analogWrite(GREEN_LED, newValueR);
  //  analogWrite(RED_LED,newValueG);
  //  analogWrite(BLUE_LED,newValueB);
}

#if INTERRUPTS
#ifdef ENCODERS
void InitialiseInterrupt() {
  cli();		// switch interrupts off while messing with their settings
  EIMSK = 0;             // Disable all interrupts
  PCICR = 0b00000010;          // Enable PCINT1 interrupt and PCINT2
  // PCMSK1 = 0b00001100;  //positions of encoders 1 and 2
  PCMSK1 = 0b00000011;  //positions of encoders PCINT8 and PCINT9
  // PCMSK2 = 0b00001000;  //position of encoder 3
  sei();		// turn interrupts back on
}

ISR(PCINT1_vect) {    // Interrupt service routine for pins with PCINT8~15
  count_encoders();
  PCIFR = 0;    // JUST FOR HEALTH OF THE CODE
}
#endif
#endif


#ifdef MUSIC
void play_rtttl(char *p)
{
  // Absolutely no error checking in here

  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while (*p != ':') p++;   // ignore name
  p++;                     // skip ':'

  // get default duration
  if (*p == 'd')
  {
    p++;
    p++;              // skip "d="
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if (num > 0) default_dur = num;
    p++;                   // skip comma
  }

  Serial.print("ddur: ");
  Serial.println(default_dur, 10);

  // get default octave
  if (*p == 'o')
  {
    p++;
    p++;              // skip "o="
    num = *p++ - '0';
    if (num >= 3 && num <= 7) default_oct = num;
    p++;                   // skip comma
  }

  Serial.print("doct: ");
  Serial.println(default_oct, 10);

  // get BPM
  if (*p == 'b')
  {
    p++;
    p++;              // skip "b="
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }

  Serial.print("bpm: ");
  Serial.println(bpm, 10);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  Serial.print("wn: ");
  Serial.println(wholenote, 10);


  // now begin note loop
  while (*p)
  {
    // first, get note duration, if available
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }

    if (num) duration = wholenote / num;
    else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch (*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if (*p == '#')
    {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if (*p == '.')
    {
      duration += duration / 2;
      p++;
    }

    // now, get scale
    if (isdigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if (*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // now play the note

    if (note)
    {
      Serial.print("Playing: ");
      Serial.print(scale, 10);
      Serial.print(' ');
      Serial.print(note, 10);
      Serial.print(" (");
      Serial.print(notes[(scale - 4) * 12 + note], 10);
      Serial.print(") ");
      Serial.println(duration, 10);
      tone(SPEAKER, notes[(scale - 4) * 12 + note]);
      delay(duration);
      noTone(SPEAKER);
    }
    else
    {
      Serial.print("Pausing: ");
      Serial.println(duration, 10);
      delay(duration);
    }
  }
}
#endif

