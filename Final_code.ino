#include<Servo.h>
#include<math.h>
#include <LiquidCrystal.h>

//--------------------------------------------------------------------------------
//------- This is the main code for the BikePacer Virtual Rival Product ----------
//-------------------- tim.linke@tum.de ----- 14.01.2021 -------------------------
//--------------------------------------------------------------------------------

//--------------------------------------
//----------- SET UP -------------------
//--------------------------------------

// BUTTONS
//Mode Button Settings
int ModeState = 0;
int ModePin = 3;
int ModeNew;
int ModeOld = 1;
//Speed Up Button
int SpeedUpState = 0;
int SpeedUpPin = 11;
int SpeedUpNew;
int SpeedUpOld = 1;
//Speed Down Button
int SpeedDownState = 0;
int SpeedDownPin = 10;
int SpeedDownNew;
int SpeedDownOld = 1;

// HELP VARIABLES
//For text which should only appear once
int intro1 = 0;
int intro2 = 0;
int finish = 0;

//DATA
//Velocity Data
int RivalV = 25;
int UserV = 19; //get data from light sensor
float UserV_float;
char RivalText1[16];
char RivalText2[4];

//MODES
//Mode Settings
int AccMode = 0;
int InstMode = 0;

//SERVO MOTOR
Servo myservo;
int servo_pin = 2;
double pos = 0;

//SENSOR
int speedSensor_pin = 12;

//LCD Display
LiquidCrystal lcd(4,5,6,7,8,9);

//TIME
int clock = 10;
double StartTime = 0;
double CurrentTime = 0;
double deltaTime = 0;

//POSITION
double laser_height = 1.01;
double circ = 2.280;
double min_distance = 0.5;
double ref_distance = 1.5;
double max_distance = 2.5;
double correction_angle = -6;
double laser_pos = 0;
double laser_realpos = 0;
double deltaD = 0;

//SPEED
double targetSpeed = 10; // in km/h
double distance_ratio = 0.1; // ratio between real distance and displayed distance
boolean v = false;
boolean lastInput = false;
boolean firstRun = true;



void setup() {
  // setup hardware
  pinMode(speedSensor_pin, INPUT);
  myservo.attach(servo_pin);
  Serial.begin(9600);
  pinMode(ModePin, INPUT);
  pinMode(SpeedUpPin, INPUT);
  pinMode(SpeedDownPin, INPUT);

  lcd.begin(16,2);

  //set laser position to starting position
  laser_pos = ref_distance;
  laser_realpos = ref_distance;
  myservo.write((180/3.1415)*atan(laser_pos/laser_height) + correction_angle);


  //First measurement is invalid. Loop ensures valid data
  while(firstRun){
    v = digitalRead(speedSensor_pin);
    //Serial.println(v);
    delay(2);
    if (v) {
      if (lastInput != v) {
        CurrentTime = millis();
        deltaTime = CurrentTime - StartTime;
        Serial.println(deltaTime);
        StartTime = CurrentTime;
        }
      firstRun = false;
    }
    lastInput = v;
  }
  
}

//--------------------------------------
//----------- ACTIONS ------------------
//--------------------------------------
void loop() {
  //check Mode
  //---------------------------------
  //------- Reading Buttons ---------
  //---------------------------------
  //Mode Button Read
  delay(10);
  ModeNew = digitalRead(ModePin);
  if(ModeOld == 0 && ModeNew == 1){
    if (ModeState == 0){
      ModeState = 1;
    }
    else if (ModeState == 1){
      ModeState = 2;
    }
    else if (ModeState == 2){
      ModeState = 3;
    }
    else {
      ModeState = 0;
    }
  }
  ModeOld = ModeNew;
  delay(10);

  //SpeedUp Button Read
  SpeedUpNew = digitalRead(SpeedUpPin);
  if(SpeedUpOld == 0 && SpeedUpNew == 1){
    if (SpeedUpState == 0){
      SpeedUpState = 1;                
    }
    else {
      SpeedUpState = 0;
      }  
  }
  SpeedUpOld = SpeedUpNew;
  delay(10);

  //SpeedDown Button Read
  SpeedDownNew = digitalRead(SpeedDownPin);
  if(SpeedDownOld == 0 && SpeedDownNew == 1){
    if (SpeedDownState == 0){
      SpeedDownState = 1;                
    }
    else {
      SpeedDownState = 0;
      }  
  }
  SpeedDownOld = SpeedDownNew;
  delay(10);

  //---------------------------------
  //------ Speed Measurement --------
  //---------------------------------
  v = digitalRead(speedSensor_pin);
  
  if (v && lastInput != v) {
  
    // set times
    CurrentTime = millis();
    deltaTime = CurrentTime - StartTime; //time between measurements
    StartTime = CurrentTime;
    deltaD = (targetSpeed*deltaTime/3600) - circ; //delta distance between set target and current speed
    //Serial.print(deltaTime);
    // set distance & angle
    laser_pos += distance_ratio * deltaD;
    laser_realpos += deltaD;
    if (laser_pos > max_distance) {
      laser_pos = max_distance;
    }
    if (laser_pos < min_distance) {
      laser_pos = min_distance;
    }
    
    // move motor
    myservo.write((180/3.1415)*atan(laser_pos/laser_height) + correction_angle);
    
    // control printouts
    //Serial.println("deltaD: ");
    //Serial.println(deltaD);
    UserV_float = (circ / (deltaTime/1000)) * 3.6;
    UserV = round(UserV_float);
    //Serial.print(UserV_float);
  
  } 
  else {
    //deltaTime = deltaTime + CurrentTime;
    }
  
  
  
  // Buttons
  if (ModeState == 0){
    //Welcome screen LCD
    lcd.setCursor(0,0);
    lcd.print("Welcome to ");
    lcd.setCursor(0,1);
    lcd.print("Virtual Rival");
    intro1 = 0; //reset intro setting
    intro2 = 0;
    finish = 0;
  }
  if (ModeState == 1){
    AccMode = 0;
    InstMode = 1;
    //Instantaneous message
    if (intro1 == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Instantaneous");
      lcd.setCursor(0,1);
      lcd.print("Mode");
      delay(2000);
      intro1 = 1;
    }
    //Home Screen LCD
    //lcd.clear();
    sprintf(RivalText1, "Rival:   %i km/h", RivalV);
    lcd.setCursor(0,0);
    lcd.print(RivalText1);
    lcd.setCursor(0,1);
    lcd.print("You:        km/h");
    //delay(500);
      //current velocity blinking
    lcd.setCursor(9,1);
    sprintf(RivalText2, "%i", UserV);
    lcd.print(RivalText2);
    //delay(500);
    //lcd.clear();
  }
  if (ModeState == 2){
    InstMode = 0;
    AccMode = 1;
    //Accumulative message
    if (intro2 == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Accumulative");
      lcd.setCursor(0,1);
      lcd.print("Mode");
      delay(2000);
      intro2 = 1;
    }
    //Home Screen LCD
    //lcd.clear();
    sprintf(RivalText1, "Rival:   %i km/h", RivalV);
    lcd.setCursor(0,0);
    lcd.print(RivalText1);
    lcd.setCursor(0,1);
    lcd.print("You:        km/h");
    //delay(500);
      //current velocity blinking
    lcd.setCursor(9,1);
    sprintf(RivalText2, "%i", UserV);
    lcd.print(RivalText2);
    //delay(500);
    //lcd.clear();
  }
  if (ModeState == 3){
    //Turn Off
    InstMode = 0;
    AccMode = 0;
    lcd.clear();
    if (finish == 0){
      lcd.print("Goodbye!");
      delay(3000);
      finish = 1;
    }
    
    lcd.clear();
    //turn laser off
  }

  if (SpeedUpState == 1){
    RivalV += 1;
    SpeedUpState = 0;
  }
  if (SpeedDownState == 1){
    RivalV -= 1;
    SpeedDownState = 0;
  }

  //Implement Button Action for SpeedUp
  //Implement Button Action for SPeedDown
  //Figure out how to pass string to LCD.print function

  
 
  lastInput = v; 
}
