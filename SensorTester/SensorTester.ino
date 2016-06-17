#define SensBR 4
#define SensFR 3
#define SensF 2
#define SensFL 1
#define SensBL 0


void setup() {
  Serial.begin (9600);
  for (int i = 0; i <= 4; i++) {
    pinMode(i + 40, OUTPUT);
    pinMode(i+30, INPUT);
  }
}

/*
 * This code checks all senseor.
 * the matrix of values will be available in 'serial monitor by the order (sensors):
 * BL | FL | F | FR | BR | White Line
 * 
 * This code is very useful and important, and we run it evey time before a coding session
 */

void loop() {
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
  if (digitalRead(47) == HIGH)Serial.print("HIGH"); //47 for VENGEANCE, 45 for ENTERPRISE
  else Serial.print("LOW");
  Serial.println();
}

int readSens(int s) {
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  //if (distance > 1914 || distance < 0) return 33;
  distance = distance / 58;
  return distance;
} 

