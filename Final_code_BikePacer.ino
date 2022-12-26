#include<Servo.h>
#include<math.h>

Servo Motor;
// Pins
int Switch_pin = 3;
int speedSensor_pin = 0;
int motor_pin = 2;
int display_pin[] = {8,7,6,9};
int latch_pin = 11;
int data_pin = 12;
int clock = 10;
int buttonInc_pin = 5;
int buttonDec_pin = 4;

//Time
double StartTime = 0;
double CurrentTime = 0;
double deltaTime = 0;

//Position
double laser_height = 1.01;
double circ = 2.280;
double min_distance = 0.5;
double ref_distance = 1.5;
double max_distance = 2.5;
double correction_angle = -6;
double laser_pos = 0;
double laser_realpos = 0;
double deltaD = 0;

//Speed
double targetSpeed = 10; // in km/h
double distance_ratio = 0.1; // ratio between real distance and displayed distance
boolean v = false;
boolean lastInput = false;
boolean firstRun = true;

//Anzeige
unsigned char numbers[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};
double increment = 0.01;



void setup() {
// setup hardware
pinMode(speedSensor_pin, INPUT);
Motor.attach(motor_pin);
Serial.begin(9600);
pinMode(Switch_pin, OUTPUT);
digitalWrite(Switch_pin, HIGH);

// Anzeige
pinMode(buttonInc_pin,INPUT_PULLUP);
pinMode(buttonDec_pin,INPUT_PULLUP);

pinMode(display_pin[0], OUTPUT);
pinMode(display_pin[1], OUTPUT);
pinMode(display_pin[2], OUTPUT);
pinMode(display_pin[3], OUTPUT);

pinMode(latch_pin, OUTPUT);
pinMode(clock, OUTPUT);
pinMode(data_pin, OUTPUT);


digitalWrite(display_pin[0], HIGH);
digitalWrite(display_pin[1], HIGH);
digitalWrite(display_pin[2], HIGH);
digitalWrite(display_pin[3], HIGH);

//set laser position to starting position
laser_pos = ref_distance;
laser_realpos = ref_distance;
Motor.write((180/3.1415)*atan(laser_pos/laser_height) + correction_angle);

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

void loop() {

//Time measurements using speed sensor
v = digitalRead(speedSensor_pin);

if (v && lastInput != v) {

// set times
CurrentTime = millis();
deltaTime = CurrentTime - StartTime;
StartTime = CurrentTime;
deltaD = (targetSpeed*deltaTime/3600) - circ;

// set distance & angle
laser_pos += distance_ratio * deltaD;
laser_realpos += deltaD;
if(laser_pos > max_distance){
laser_pos = max_distance;
}
if(laser_pos < min_distance){
laser_pos = min_distance;
}

// move motor
Motor.write((180/3.1415)*atan(laser_pos/laser_height) + correction_angle);

// reactivate 5V
//digitalWrite(Switch_pin, HIGH);

// control printouts
Serial.println("deltaD: ");
Serial.println(deltaD);

} 
else {
if (millis() - StartTime > 5000){
digitalWrite(Switch_pin, LOW);
Serial.println("Off");
}
}



// Buttons
if(digitalRead(buttonInc_pin) == LOW){
targetSpeed = targetSpeed + increment;
}
if(digitalRead(buttonDec_pin) == LOW){
targetSpeed = targetSpeed - increment;
}
displayNumber(targetSpeed*100,2);

lastInput = v; 
}



void displayNumber(int N, int Point){

for(int i=0; i<4; i++){

if(i==Point){
Display(N%10,128);
}
else {
Display(N%10,0);
}
digitalWrite(display_pin[3-i], LOW);
delay(1000);
digitalWrite(display_pin[3-i], HIGH);
N = (N - (N%10)) / 10; 
}

}

void Display(int num, char point){

digitalWrite(latch_pin, LOW);
shiftOut(data_pin, clock, MSBFIRST, numbers[num]+point);
digitalWrite(latch_pin, HIGH);

}
