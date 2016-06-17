
#define SensBR 4
#define SensFR 3
#define SensF 2
#define SensFL 1
#define SensBL 0

const int pwmA = 3, //A2   //edited@ 19/05/16
          pwmB = 11, //B2
          dirA = 12, //A1
          dirB = 13, //B1

          FS = 20, //Forward sensor distance to wall
          Speed = 37,   // 37 for full battary
          BSMIN = 6;  //TODO try 7
const int pin = 46;

/* forward() is a method to drive between to walls
   basically, the robot go straight for 140 mSeconds
   then faces 2-conditions to fix it's course(and checks it for each time):
   -is the robot have a tendency towards a wall(check the Forward&(Right/Left) sensor with FS value)
   -is the robot is now too close to the wall (check the Back&(Right/Left) sensor with BSMIN value)
   then fixes to the other side or finishes it's actions.

   forward has to be in a loop, of any kind, and its operation time is bi-constant (140 or 300 mSeconds).

   readSens(sensor) might be very slow for long distance. readSens(sensor, maxDistance) has better performance time but limited top distance(maxDistance)
*/

void setup() {
  pinMode(46, OUTPUT);
  Serial.begin (9600);
  for (int cn = 0; cn <= 4; cn++) {
    pinMode(cn + 40, OUTPUT);
    pinMode(cn + 30, INPUT);
  }
  pinMode(45, INPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);

  sensorCalibration();
}

void loop() {
  forward();
}

void forward() {
  //TODO remove 2 lines:
  forwardOld();
  delay(140);    //140 for full battary

  if (readSens(SensFR) < FS || readSens(SensBR) < BSMIN ) {
    fixR();
    delay(160);  //160 for full battary

  }
  else if (readSens(SensFL) < FS || readSens(SensBL) < BSMIN ) {
    fixL();
    delay(160);   //160 for full battary
  }
}

void stop() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, 0);
}



void fixL() {                   // turns to the right
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 15);
  analogWrite (pwmB, 44);
}
void fixR() {                   // turns to the left
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 40);
  analogWrite (pwmB, 15);
}

void forwardOld()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed );
  analogWrite (pwmB, Speed + 2);
}
void stopN() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, 0);
}
int readSens(int s) {
  int Eb4 = 311, G4 = 392;
  int x = 250;
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  distance = distance / 58;
  if (distance == 0) {  //if dis==0, then this sensor is disconnected
    stopN();
    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s);
  }
  return distance;
}


int readSens(int s, int maxDistance) {
  int Eb4 = 311, G4 = 392;
  int x = 250;
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  if (maxDistance * 58 < distance) return maxDistance;
  distance = distance / 58;
  if (distance == 0) {  //if dis==0, then this sensor is disconnected
    stop();
    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s, maxDistance);
  }
  return distance;
}

void sensorCalibration() {
  Serial.print(readSens(0));
  Serial.print(" | ");
  Serial.print(readSens(1));
  Serial.print(" | ");
  Serial.print(readSens(2));
  Serial.print(" | ");
  Serial.print(readSens(3));
  Serial.print(" | ");
  Serial.print(readSens(4));
  Serial.print(" | ");
  if (digitalRead(45) == HIGH)Serial.println("HIGH"); //47 for VENGEANCE, 45 for ENTERPRISE  //White Line Sensor
  else Serial.println("LOW");
}

