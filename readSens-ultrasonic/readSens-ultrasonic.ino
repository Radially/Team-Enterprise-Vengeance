
#define SensBR 4
#define SensFR 3
#define SensF 2
#define SensFL 1
#define SensBL 0

const int pwmA = 3, //A2   //edited@ 19/05/16
          pwmB = 11, //B2
          dirA = 12, //A1
          dirB = 13, //B1
          pin = 46;

/* this method allows you to read distance from an ultrasonic sensor.
    To optimize running time, a maximum-relevant-distance-ceiling is set. it worth it.

    noted: a part that deals with situations of 0 (the sensor is disconnected). the program stops for 250 mSecs ,beeps, and tries again.
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
}

void loop() {
  sensorCalibration();
}
int readSens(int trigger, int echo , int maxDistance) {
  int Eb4 = 311, G4 = 392;
  int x = 250;
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, HIGH);
  int distance = pulseIn(echo, HIGH);
  if (maxDistance * 58 < distance) return maxDistance;
  distance = distance / 58;
  /*if (distance == 0) {  //if dis==0, then this sensor is disconnected
    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s, maxDistance);
    }*/
  return distance;
}



int readSens(int s, int maxDistance) {
  return readSens(s+40, s+30, maxDistance);
}

int readSens(int s) {
  return readSens(s+40, s+30, 255);
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

