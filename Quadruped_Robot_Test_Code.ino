#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "dataArray.h"
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int servonum = 12;
int randnumber;
const float servodeg0[12] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90};
float servodegnew[servonum]; // The desired servo position in degrees
float servodegold[servonum]; // The old (or current) servo position
const int servodir[12] = {  +1, +1, -1, -1, -1, +1, -1, -1, -1, +1, +1, +1}; // Turning direction (positive is servo counter-clockwise)
const float pi = 3.1416;
const float alfa0 = pi / 6; // The neutral position of alfa (30 deg)
const float beta0 = pi / 3; // The neutral position of beta (60 deg)
const float jointlength = 50; // The length of a leg part (both have the same length)
const float width = 120; // The width (distance between feet in y direction, with toeout0 added)
const float leng = 120; // The length (disatnce between feet in x direction)
const float distag = 12; // Distance between alfa and gamma axis
const float toeout0 = 20; // The outward distance of feet from the gamma servo centre (the distance the foot is pointed outwards)
const float leglength0 = 2 * jointlength * cos(alfa0);
const float gamma0 = asin(toeout0 / (leglength0 + distag)); // The neutral position of gamma (due to toe out 20 mm and distag 12 mm)
const float bodyradius = sqrt(pow((width / 2), 2) + pow((leng / 2), 2)); // The length of diagonal (distance from centre to foot corner)
const float phi0 = atan(width / leng); // The angle of bodyradius vs the x (forward pointing) axis
const float height0 = sqrt(pow(leglength0 + distag, 2) - pow(toeout0, 2)); // The normal height of robot (if any angles or distances are changed this must be updated)
float leglength [4] = {sqrt(pow(height0, 2) + pow(toeout0, 2)), sqrt(pow(height0, 2) + pow(toeout0, 2)),
                       sqrt(pow(height0, 2) + pow(toeout0, 2)), sqrt(pow(height0, 2) + pow(toeout0, 2))
                      };
// Start values of leglength
unsigned long timestep = 500; // Time taken by each sequence (when using servomove())
int steplength = 70; //40//The length of a step in x direction during walking (forward and reverse creep)
float phi = 20; //20// turnangle during turning (in degrees, not radians!)
// Variable for movement
float footpos[12]; // Foot positions, order LeftFrontxyz, LeftRearxyz, RightFrontxyz, RightRearxyz
float stepturn[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Foot movement in case of a turn
// The foot positions are calibrated with their respective start positions
const float jointangle0[12] = {alfa0, beta0, 0, alfa0, beta0, 0, alfa0, beta0, 0, alfa0, beta0, 0};
float jointangle[12]; //Using a vector for angles, order LeftFrontAlfaBetaGamma etc
#define SERVOMIN  150  // Minimum pulse length in microseconds
#define SERVOMAX  600  // Maximum pulse length in microseconds
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  delay(10);

  for (int i = 0; i < servonum; i++) {    //servonum=12
    servodegnew[i] = servodeg0[i];     //servodeg0[12] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90}
    servodegold[i] = servodegnew[i];
    pwm.setPWM(i, 0, map(servodegnew[i], 0, 180, SERVOMIN, SERVOMAX));
  } 

  delay(10000);
}

void loop() {

  // this loop defines the basic forward, 90 degree left turn, and 90 degree right turn functions of the robot
  for (int j =0; j<= arrayLength; j++ ){
    if (arr[j]==1){
      Serial.println("Moving down");
      forwardcreep();
      delay(500);
      forwardcreep();
      delay(500);
      forwardcreep();
      delay(1000);
    }
    if (arr[j]==0){
      Serial.println("Moving left");
      leftturn90deg();
      delay(300);
      forwardcreep();
      delay(200);
      forwardcreep();
      delay(200);
      forwardcreep();
      delay(200);
      rightturn90deg();
      delay(1000);
    }
    if (arr[j]==2){
      Serial.println("Moving right");
      rightturn90deg();
      delay(300);
      forwardcreep();
      delay(200);
      forwardcreep();
      delay(200);
      forwardcreep();
      delay(200);
      leftturn90deg();
      delay(1000);
    }
    if (arr[j]==3){
      Serial.println("Moving up");
      reversecreep();
      delay(500);
      reversecreep();
      delay(500);
      reversecreep();
      delay(1000);
    }
  } 
}

void moveStop() {
  // Stop all servos
  for (int i = 0; i < servonum; i++) {
    int pulseLength = angleToPulse(90); // 90 degrees is the center position for most servos
    pwm.setPWM(i, 0, pulseLength);
  }
}

void lengthangles() {
  // Front left foot
  jointangle[2] = gammaleft(footpos[1], footpos[2]);
  leglength[0] = legleft(footpos[0], footpos[2], jointangle[2]);
  jointangle[1] = beta(leglength[0]);
  jointangle[0] = alfafront(footpos[0], jointangle[1], leglength[0]);
  // Rear left foot
  jointangle[5] = gammaleft(footpos[4], footpos[5]);
  leglength[1] = legleft(footpos[3], footpos[5], jointangle[5]);
  jointangle[4] = beta(leglength[1]);
  jointangle[3] = alfarear(footpos[3], jointangle[4], leglength[1]);
  // Front rigth foot
  jointangle[8] = gammaright(footpos[7], footpos[8]);
  leglength[2] = legright(footpos[6], footpos[8], jointangle[8]);
  jointangle[7] = beta(leglength[2]);
  jointangle[6] = alfafront(footpos[6], jointangle[7], leglength[2]);
  // Rear right foot
  jointangle[11] = gammaright(footpos[10], footpos[11]);
  leglength[3] = legright(footpos[9], footpos[11], jointangle[11]);
  jointangle[10] = beta(leglength[3]);
  jointangle[9] = alfarear(footpos[9], jointangle[10], leglength[3]);
}

float gammaleft (float dy, float dz) {
  float gresult = atan((toeout0 + dy) / (height0 - dz)) - gamma0;
  return gresult;
}

float gammaright(float dy, float dz) {
  float gresult = gamma0 - atan((toeout0 - dy) / (height0 - dz));
  return gresult;
}

//Calculating leg length (distance alfa axis to toe)
float legleft(float dx, float dz, float gamma) {
  float lresult = sqrt(pow(leglength0 - (dz / cos(gamma0 + gamma)), 2) + pow(dx, 2));
  if (lresult > 2 * jointlength) lresult = 2 * jointlength; // If leglength is higher than possible some following functions become unstable
  return lresult;
}

float legright(float dx, float dz, float gamma) {
  float lresult = sqrt(pow(leglength0 - (dz / cos(gamma0 - gamma)), 2) + pow(dx, 2));
  if (lresult > 2 * jointlength) lresult = 2 * jointlength; // If leglength is higher than possible some following functions become unstable
  return lresult;
}

// Beta, the "knee joint"
float beta(float leg) {
  float bresult = 2 * acos(leg / (2 * jointlength));
  return bresult;
}

// Alfa, The other hip servo
float alfafront(float dx, float beta, float leg) {
  float aresult = (beta / 2) - asin(dx / leg);
  return aresult;
}

float alfarear(float dx, float beta, float leg) {
  float aresult = (beta / 2) + asin(dx / leg);
  return aresult;
}

// Giving foot positions based on a turning angle f (in degrees). Stepturn is the used to make footpos values
void turnpos(float f) {
  stepturn[0] = bodyradius * cos(phi0 + (f * pi / 180)) - leng / 2;
  stepturn[1] = bodyradius * sin(phi0 + (f * pi / 180)) - width / 2;
  stepturn[3] = bodyradius * cos(pi - phi0 + (f * pi / 180)) + leng / 2;
  stepturn[4] = bodyradius * sin(pi - phi0 + (f * pi / 180)) - width / 2;
  stepturn[6] = bodyradius * cos(2 * pi - phi0 + (f * pi / 180)) - leng / 2;
  stepturn[7] = bodyradius * sin(2 * pi - phi0 + (f * pi / 180)) + width / 2;
  stepturn[9] = bodyradius * cos(pi + phi0 + (f * pi / 180)) + leng / 2;
  stepturn[10] = bodyradius * sin(pi + phi0 + (f * pi / 180)) + width / 2;
}

// Calculates servo positions (in degrees) based on joint angles in the fucntion above
void servopos() {
  for (int i = 0; i < 12; i++) {
    servodegnew[i] = servodeg0[i] + servodir[i] * (jointangle[i] - jointangle0[i]) * 180 / pi;
  }
}

// The servo algorithm for controlled and syncronized movements. All servos should reach their end position at the end of a timestep
void servomove() {
  int servotimeold[servonum]; // Local variable for time of last servo position
  int servotimenew[servonum]; // Local variable for the current time when the servo i positioned
  int SERVOPULSE[servonum]; // Local variable to write to the servo driver
  float servodeg[servonum]; // Local variable for the current servo position
  float servodegspeed[servonum]; // Local variable for the desired servo speed degress per millisecond
  unsigned long starttime = millis(); // Time stamp the start of the algorithm
  unsigned long timenow = starttime; // Resetting time now
  for (int i = 0; i < servonum; i++) {
    servodegspeed[i] = (servodegnew[i] - servodegold[i]) / timestep; // Calculate the desired servo speed
    servodeg[i] = servodegold[i]; // Resetting the servo position
    servotimeold[i] = starttime; // Resetting the time
  }
  while ((timenow - starttime) < timestep) { // Loop continues until the time step is fulfilled
    for (int i = 0; i < servonum; i++) { // Iterate through each servo
      servotimenew[i] = millis(); // Get a time stamp
      servodeg[i] += servodegspeed[i] * (servotimenew[i] - servotimeold[i]);
      // Calculate a new position based on the desired speed and elapsed time
      int pulseLength = angleToPulse(servodeg[i]); // Convert the angle to pulse length
      pwm.setPWM(i, 0, pulseLength); // Apply the PWM signal
      servotimeold[i] = servotimenew[i];
    }
    timenow = millis();
    // Get a time stamp after all servos has been iterated to use in the while case.
  }
  for (int i = 0; i < servonum; i++) { // Make on last iteration to assure that the servos reached their end positions
    int pulseLength = angleToPulse(servodeg[i]); // Convert the angle to pulse length
    pwm.setPWM(i, 0, pulseLength); // Apply the PWM signal
    servotimeold[i] = servotimenew[i];
  }
}

void servomovefast() {
  for (int i = 0; i < servonum; i++) { // Make on last iteration to assure that the servos reached their end positions
    int pulseLength = angleToPulse(servodegnew[i]); // Convert the angle to pulse length
    pwm.setPWM(i, 0, pulseLength); // Apply the PWM signal
    servodegold[i] = servodegnew[i]; // Resetting the current position for future iterations
  }
  delay(100); // Just give a reasonable time for servos to reach endpoint before moving on.
}

// Calculates a foot position (xyz coordiantes)
void footxyz(int i, float x, float y, float z) {
  footpos[3 * i] = x;
  footpos[3 * i + 1] = y;
  footpos[3 * i + 2] = z;
  lengthangles();
  servopos();
}

// Calculates foot movement, adding desired value to current position
void footmovexyz(int i, float x, float y, float z) {
  footpos[3 * i] += x;
  footpos[3 * i + 1] += y;
  footpos[3 * i + 2] += z;
  lengthangles();
  servopos();
}

// Calculates body positioning according to xyz coordinates.
void bodyxyz(float x, float y, float z ) {
  //Note: Moving body means moving the feet in the other direction, hence minus signs in all foot positions
  for (int i = 0; i < 4; i++) {
    footpos[3 * i] = -x;
    footpos[3 * i + 1] = -y;
    footpos[3 * i + 2] = -z;
  }
  lengthangles();
  servopos();
}

// Calculates body movement, adding cooridinate to existing position.
void bodymovexyz(float x, float y, float z ) {
  //Note: Body move mean moving the feet in the other direction, hence minus signs in all foot positions
  for (int i = 0; i < 4; i++) {
    footpos[3 * i] -= x;
    footpos[3 * i + 1] -= y;
    footpos[3 * i + 2] -= z;
  }
  lengthangles();
  servopos();
}

// Calculates a twist on the body the desired angle phi
void bodytwist(float f) {
  // Again here the movement is in minus driection from the foot positions
  turnpos(-f);
  for (int i = 0; i < 12; i++) {
    footpos[i] += stepturn[i];
  }
  lengthangles();
  servopos();
}

// Does a footmovement; lifts move xy and puts down foot
void footstep (int i, float x, float y) {
  footmovexyz(i, 0, 0, 30);
  servomovefast();
  footmovexyz(i, x, y, 0);
  servomovefast();
  footmovexyz(i, 0, 0, -30);
  servomovefast();
}

// Does a footmovement based on the disired turning angle, moves the foot along the turning arc
void (footstepangle(int i, float f)) {
  turnpos(f);
  footmovexyz(i, 0, 0, 30);
  servomovefast();
  footmovexyz(i, stepturn[3 * i], stepturn [3 * i + 1], 0);
  servomovefast();
  footmovexyz(i, 0, 0, -30);
  servomovefast();
}

// A gait for forward creeping
void forwardcreep() {
  bodymovexyz(steplength / 4, -toeout0, 0); // Starts to position for forward walking, leaning to the right
  servomove();
  footstep(1, steplength / 2, 0); // Moving rear left leg one half step length
  footstep(0, steplength / 2, 0); // And the front left
  bodymovexyz(steplength / 4, 2 * toeout0, 0); // Shifting body forward and to the left (in order to move the right feet later)
  servomove();
 {
    // Here the while loop starts, repeaetd as long as fwd is ordered (mode 1)
    footstep(3, steplength, 0); // Moving rear right forward
    footstep(2, steplength, 0); // Moving front right forward
    bodymovexyz(steplength / 2, -2 * toeout0, 0); // Shifting body forward and to the right
    servomove();
    footstep(1, steplength, 0); // Moving rear left forward
    footstep(0, steplength, 0); // Moving front left forward
    bodymovexyz(steplength / 2, 2 * toeout0, 0); // Shifting body forward and to the left
    servomove();
 }
    // The robot has the same position as before the while loop but has moved on steplength forward.
  // The while loop ends and it assumes normal postion
  /* bodymovexyz(0, 10, 0);*/
  footstep(3, steplength / 2, 0); // Taking half steps to make all legs neutral
  footstep(2, steplength / 2, 0);
  bodyxyz(0, 0, 0); // Centering body
  servomove();
  // Leaving gait mode
}

// A similar gait for reverse walking (not commented as much look at forwardcreep
void reversecreep() {
  bodymovexyz(-steplength / 4, -toeout0, 0); // Starts to position for forward walking
  servomove();
  footstep(0, -steplength / 2, 0);
  footstep(1, -steplength / 2, 0);
  bodymovexyz(-steplength / 4, 2 * toeout0, 0);
  servomove();
{
    // Here the while loop starts, repeaetd as long as reverse is ordered (mode 2)
    footstep(2, -steplength, 0);
    footstep(3, -steplength, 0);
    bodymovexyz(-steplength / 2, -2 * toeout0, 0);
    servomove();
    footstep(0, -steplength, 0);
    footstep(1, -steplength, 0);
    bodymovexyz(-steplength / 2, 2 * toeout0, 0);
    servomove();
  }
  // The while loop ends and it assumes normal postion
  /*  bodymovexyz(0, 10, 0);*/
  footstep(2, -steplength / 2, 0);
  footstep(3, -steplength / 2, 0);
  bodyxyz(0, 0, 0);
  servomove();
  // Leaving gait mode
}

// Doing a turn to the left the desired phi angle
void leftturn() {
{
    // While loop as long as the left button is pressed
    bodyxyz(toeout0 / 2, toeout0, 0); // Lean left before doing anything
    servomove();
    footstepangle(3, phi); // Move rear right foot into new position
    footstepangle(2, phi); // Move front right foot into new position
    footxyz(0, -toeout0 / 2 - stepturn[0], toeout0 - stepturn[1], 0);
    footxyz(1, -toeout0 / 2 - stepturn[3], toeout0 - stepturn[4], 0);
    footxyz(2, -toeout0 / 2, toeout0, 0);
    footxyz(3, -toeout0 / 2, toeout0, 0);
    // Twisting body and lean left. Written in absolute coordinates to minmize errors.
    servomove(); // Do the actual servo command
    footstepangle(0, phi); // Move front left foot
    footstepangle(1, phi); // Move rear left foot
  }
  bodyxyz(0, 0, 0); // Centre body when turning is finished
  servomove();
}

void leftturn90deg(){
  for (int i = 0; i < 8; i++) {
    leftturn();
  }
}

//Doing a right turn. Should be identical to left turn but with different commands. Testing both at the moment.
void rightturn() {
{
    // While loop as long as the right button is pressed
    bodyxyz(-toeout0 / 2, toeout0, 0); // Lean left before doing anything
    servomove();
    footstepangle(2, -phi); //Move front right foot
    footstepangle(3, -phi); //Move rear right foot
    footxyz(0, toeout0 / 2 - stepturn[0], toeout0 - stepturn[1], 0);
    footxyz(1, toeout0 / 2 - stepturn[3], toeout0 - stepturn[4], 0);
    footxyz(2, toeout0 / 2, toeout0, 0);
    footxyz(3, toeout0 / 2, toeout0, 0);
    // Twisting body and lean left. Written in absolute coordinates to minmize errors.
    servomove(); // Do the actual servo command
    footstepangle(1, -phi); //Move rear left foot
    footstepangle(0, -phi); //Move front left foot
  }
  bodyxyz(0, 0, 0);
  servomove();
}

void rightturn90deg(){
  for (int i = 0; i < 16; i++) {  //changed to 16
    rightturn();
  }
}