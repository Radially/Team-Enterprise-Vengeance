#define SensBR 4
#define SensFR 3
#define SensF 2
#define SensFL 1
#define SensBL 0

const int pwmA = 3, //A2
          pwmB = 11, //B2
          dirA = 12, //A1
          dirB = 13, //B1
          pin = 46,

          BS = 25, //Backward sensor distance to wall
          FS = 15, //Forward sensor minimum distance to wall
          VFS = 8, //Very Forward sensor distance to wall
          FSMAX = 21,
          Speed = 40,
          MinDis = 20;


void setup() {
  Serial.begin (9600);
  for (int i = 0; i <= 4; i++) {
    pinMode(i + 40, OUTPUT);
    pinMode(i + 30, INPUT);
  }
  pinMode(46, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);

  readSens(SensF);
  readSens(SensBR);
  readSens(SensBL);
}


/* This code has been used to demonstrate avoiding collisions in a room.

   The robot goes straight until faces a wall.
   Then, if a left and right turn are both irrelevant, the robot turns back.
   Else, it decides which side has greater potential to go straight towards it, and turns.

   Consider using readSens(sensor, maxDistance), since readSens(sensor) might make strange delays,
   especially where (readSens(SensF)<MinDis), and maxDistance can be MinDis+1.

   Consider using turnRU(), turnLU() and turnB() from enterprise competition code, but remove the forward() from their ends.


   Troubleshooting:
 * *if the robot beeps and stops - a sensor is disconnected, run SensorTester.

*/


void loop() {
  forwardOld();
  delay(100);
  if (readSens(SensF) < MinDis) {
    if (readSens(SensBL) < MinDis && readSens(SensBR) < MinDis) {
      turnB();
    }
    else if (readSens(SensBL) < readSens(SensBR)) {
      turnR();
    }
    else {
      turnL();
    }
  }
}

void turnR() {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, Speed);
  delay(1000);

}
void turnL() {
  // digitalWrite(7, HIGH);
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, 0);
  delay(900);

}
void turnB() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, Speed);
  delay(650);
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed + 8);
  analogWrite (pwmB, Speed + 8);
  delay(750);
}

void stopN() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, 0);
}
void forwardOld()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed + 2);
  analogWrite (pwmB, Speed);
}

int readSens(int s) {
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  distance = distance / 58;
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
    stopN();
    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s , maxDistance);
  }
  return distance;
}

