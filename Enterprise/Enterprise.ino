
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

          BS = 27 , //Backward sensor distance to wall (bigger value - turn!)
          FS = 20, //Forward sensor distance to wall (smaller value - fix to the other side (from forward());
          VFS = 10, //Very Forward sensor distance to wall (notice wall ahead, consider 3 or 4)
          FSMAX = 30, //maximum relevant distance (it is possible to go straight in this junction)
          FSSMART = 15,  //Maximum distrance to continue with a smart turn (turnRU(), turnLU);
          Speed = 37,   // 37 for full battary
          BS2T = 18,  //Private case of BS to turn left
          BSMIN = 6;  //Minimal back sensor distance from a wall (used in forward())


//make the turns array:
int const length = 18;
int a[length];
int i = 0;

void setup() {
  for (int v = 0; v < length; v++) a[v] = 0;
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(pin, OUTPUT);
  Serial.begin (9600);

  //set sensors
  for (int cn = 0; cn <= 4; cn++) {
    pinMode(cn + 40, OUTPUT);
    pinMode(cn + 30, INPUT);
  }
  pinMode(45, INPUT);
  //set motors
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);


  //set comunication
  initialzeTrans();
  //run one sensor check(which fails always, don't worry)
  sensorCalibration();
}

/*
   This is the code from the comprtition in Hadera, 5.6.16.
   note that the forwardOld() must drive the robot STRAIGHT, else mistakes happends. so change the + or - there (+2, +1, -1, -2 are good values)
   it will get a rotation when the voltage of the batteries changes.
   first and second run (of a new battery) may be too strong, don't be afraid of it.

*/

void loop() {
  while (digitalRead(45) == HIGH) nav();     // 1 on black //Drive untill you're over a white area
  a[i] = 0;  //remember 0, to mark maze end
  while (readSens(SensF) > BSMIN) forwardOld();  //park
  turnR();
  stop();
  //delay(5000);  //what? come on! now i understand why sending the path took so much time...
  
  optimization(a);
  for (int c = 0; c < length; c ++) {  //send path
    pack(a[c], 0);
    delay(500);
    sendPackage();
  }

  musicTime();
  delay(60000);
}


void nav() {
  forward();
  if (readSens(SensBR) > BS) {
    turnRU();
    a[i] = 1;
    Serial.println(a[i]);
    i++;
  }
  else if (readSens11(SensF) < VFS) {
    //Serial.println(readSens(3));  //19_05
    if (readSens21(SensFR) > FS /*+ 2*/) {
      turnRU();
      a[i] = 1;
      Serial.println(a[i]);
      i++;
    }
    else if (readSens21(SensBL) > BS2T || readSens21(SensFL) > FS + 2) {
      turnLU();
      a[i] = 3;
      Serial.println(a[i]);
      i++;
    }
    else {
      turnB();
      a[i] = 4;
      Serial.println(a[i]);
      i++;
    }
  }
  else if (readSens(SensBL) > BS && readSens(SensF) > FSMAX) {  //turn straight
    do {
      tone(pin, 260);
      forward();
    }
    while (readSens(SensBL) > FSSMART /*BS*/) ;
    a[i] = 2;
    Serial.println(a[i]);
    noTone(pin);
    i++;
    forward();
    forward();
    forward();
  }
  else {
    //forward();
  }
}


void forward() {
  forwardOld();
  delay(140);    //140 for full battary

  if (readSens21(SensFR) < FS || readSens11(SensBR) < BSMIN ) {
    fixR();
    delay(160);  //180 for full battary

  }
  else if (readSens21(SensFL) < FS || readSens11(SensBL) < BSMIN ) {
    fixL();
    delay(160);   //180 for full battary
  }
}

void turnB() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, Speed);
  delay(700);  //700 for full battary
  if (readSens11(SensBL) < BSMIN) { //not clockwise please
    digitalWrite(dirA, HIGH);
    digitalWrite(dirB, LOW);
    analogWrite (pwmA, 0);
    analogWrite (pwmB, Speed - 3);
    delay(1600);  //1222 for full battary
    while (readSens(SensFR) < FS || readSens(SensF) < FS) {
      tone (pin, 450);
      delay(80);
      noTone(pin);
    }
  }
  else if (readSens11(SensBR) < BSMIN) { //not clockwise please
    digitalWrite(dirA, LOW);
    digitalWrite(dirB, LOW);
    analogWrite (pwmA, Speed - 3);
    analogWrite (pwmB, 0);
    delay(1600);  //1222 for full battary
    while (readSens(SensFR) < FS || readSens(SensF) < FS) {
      tone (pin, 450);
      delay(80);
      noTone(pin);
    }
  }
  else if (readSens21(SensBL) > readSens21(SensBR)) { //not clockwise please
    digitalWrite(dirA, HIGH);
    digitalWrite(dirB, LOW);
    analogWrite (pwmA, Speed - 3);
    analogWrite (pwmB, Speed - 3);
    delay(800);  //1222 for full battary
    while (readSens(SensFR) < FS || readSens(SensF) < FS) {
      tone (pin, 450);
      delay(80);
      noTone(pin);
    }
  }
  else {                             //Yes clockwise please
    digitalWrite(dirA, LOW);
    digitalWrite(dirB, HIGH);
    analogWrite (pwmA, Speed - 3);
    analogWrite (pwmB, Speed - 3);
    delay(800);  //1222 for full battary
    while (readSens(SensFL) < FSSMART || readSens(SensF) < FSSMART) {
      tone (pin, 450);
      delay(80);
      noTone(pin);
    }
  }
}

void turnRU() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, Speed);
  delay(750);  //1000 for full battary
  while (readSens(SensFL) < FSSMART  ||  readSens(SensF) < BS) {
    if (readSens(SensFR) < FSMAX) break;
    tone (pin, 450);
    delay(70);
    noTone(pin);
  }
  /*forwardOld();
    delay(1000);  //changed to 1000, need to be 1200*/
  forward();
  forward();
  forward();
  forward();
  forward();
}
void turnLU() {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, 0);
  delay(650);    //950 for full battary
  while (readSens(SensFR) < FSSMART || readSens(SensF) < BS) {
    if (readSens(SensFL) < FSMAX) break;
    tone (pin, 450);
    delay(70);
    noTone(pin);
  }
  /*forwardOld();
    delay(1000);  //changed to 1000, need to be 1200*/
  forward();
  forward();
  forward();
  forward();
  forward();
}

void turnRUJunior() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 10);
  analogWrite (pwmB, Speed);
  delay(650);  //1000 for full battary
  while (readSens(SensFL) < FSSMART  ||  readSens(SensF) < BS) {
    tone (pin, 450);
    delay(100);
    noTone(pin);
  }
  forwardOld();
  delay(800);  //changed to 1000, need to be 1200
  forward();
}
void turnLUJunior() {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, 10);
  delay(550);    //950 for full battary
  while (readSens(SensFR) < FSSMART || readSens(SensF) < BS) {
    tone (pin, 450);
    delay(60);
    noTone(pin);
  }
  forwardOld();
  delay(800);   //changed to 1000, need to be 1200
  forward();
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

void fixB() {                   // reverse for small distance
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, Speed);
}
void forwardOld()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed );
  analogWrite (pwmB, Speed + 2);
}

void turnR() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, Speed);
  delay(1000);  //1000 for full battary
  /*forwardOld();
    delay(1500);*/
}
void turnL() {
  // digitalWrite(7, HIGH);
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, 0);
  delay(950);    //950 for full battary
  // digitalWrite(7, LOW);
  /*forwardOld();
    delay(1500);*/
}


void optimization(int a[]) {
  int i;
  boolean b = true;
  while (b) {
    b = false;
    i = 0;
    while (a[i] != 0) {
      do i++;
      while (a[i] != 4 && a[i] != 0);
      if (a[i] == 4) {
        b = true;
        a[i - 1] += a[i + 1];
        for (int n = i; n < 11; n++) {
          a[n] = a[n + 2];
        }
      }
    }
  }
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
  if (distance > 1914) return 33;
  distance = distance / 58;
  if (distance == 0) {
    stop();

    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s);
  }
  return distance;
}

int readSens21(int s) {
  int Eb4 = 311, G4 = 392;
  int x = 250;
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  if (distance > 1218) return 21;
  distance = distance / 58;
  if (distance == 0) {
    stop();

    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s);
  }
  return distance;
}

int readSens11(int s) {
  int Eb4 = 311, G4 = 392;
  int x = 250;
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  if (distance > 638) return 11;
  distance = distance / 58;
  if (distance == 0) {
    stop();

    tone(pin, G4);
    delay(x);
    noTone(pin);
    return readSens(s);
  }
  return distance;
}

/////////////////////////////////////   communication    /////////////////////////////////////////////
void initialzeTrans() {
  pinMode(2, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(13, LOW);
}

void pack(int n1, int n2) {
  switch (n1) {
    case 0:
      digitalWrite(2, LOW);
      digitalWrite(4, LOW);
      break;
    case 1:
      digitalWrite(2, LOW);
      digitalWrite(4, HIGH);
      break;
    case 2:
      digitalWrite(2, HIGH);
      digitalWrite(4, LOW);
      break;
    case 3:
      digitalWrite(2, HIGH);
      digitalWrite(4, HIGH);
      break;
  }
  switch (n2) {
    case 0:
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      break;
    case 1:
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      break;
    case 2:
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      break;
    case 3:
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      break;
  }
}

void sendPackage() {
  digitalWrite(7, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  digitalWrite(7, HIGH);
  //delay(1000);
  digitalWrite(13, LOW);
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
  if (digitalRead(45) == HIGH)Serial.println("HIGH"); //47 for VENGEANCE, 45 for ENTERPRISE
  else Serial.println("LOW");
}

////////////////////////////////////////////   MUSIC!!! MTV ///////////////////////////////////////////////////////////
void musicTime() {  // Darth Vader
  int x = 125;

  int C4 = 262, E4 = 330, Eb4 = 311, B4 = 494, Bb4 = 466, G4 = 392, D5 = 587, Eb5 = 622, Gb4 = 370, G5 = 784, Gb5 = 740, F5 = 698, E5 = 659, Gd4 = 415, Cd5 = 554, C5 = 523, A4 = 440;


  tone (pin, G4);
  delay(x * 4);
  noTone(pin);
  tone (pin, G4);
  delay(x * 4);
  noTone(pin);
  tone (pin, G4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);

  tone (pin, G4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, G4);
  delay(x * 8);
  noTone(pin);

  tone (pin, D5);
  delay(x * 4);
  noTone(pin);
  tone (pin, D5);
  delay(x * 4);
  noTone(pin);
  tone (pin, D5);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb5);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);

  tone (pin, Gb4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, G4);
  delay(x * 8);
  noTone(pin);

  tone (pin, G5);
  delay(x * 4);
  noTone(pin);
  tone (pin, G4);
  delay(x * 3);
  noTone(pin);
  tone (pin, G4);
  delay(x);
  noTone(pin);
  tone (pin, G5);
  delay(x * 4);
  noTone(pin);
  tone (pin, Gb5);
  delay(x * 3);
  noTone(pin);
  tone (pin, F5);
  delay(x);
  noTone(pin);

  tone (pin, E5);
  delay(x);
  noTone(pin);
  tone (pin, Eb5);
  delay(x);
  noTone(pin);
  tone (pin, E5);
  delay(x * 2);
  noTone(pin);
  delay(2 * x);
  tone (pin, Gd4);
  delay(x * 2);
  noTone(pin);
  tone (pin, Cd5);
  delay(x * 4);
  noTone(pin);
  tone (pin, C5);
  delay(x * 3);
  noTone(pin);
  tone (pin, B4);
  delay(x);
  noTone(pin);

  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, A4);
  delay(x);
  noTone(pin);
  tone (pin, Bb4);
  delay(x * 2);
  noTone(pin);
  delay(2 * x);
  tone (pin, Eb4);
  delay(x * 2);
  noTone(pin);
  tone (pin, Gb4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Gb4);
  delay(x);
  noTone(pin);

  tone (pin, Bb4);
  delay(x * 4);
  noTone(pin);
  tone (pin, G4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, D5);
  delay(x * 8);
  noTone(pin);

  tone (pin, G5);
  delay(x * 4);
  noTone(pin);
  tone (pin, G4);
  delay(x * 3);
  noTone(pin);
  tone (pin, G4);
  delay(x);
  noTone(pin);
  tone (pin, G5);
  delay(x * 4);
  noTone(pin);
  tone (pin, Gb5);
  delay(x * 3);
  noTone(pin);
  tone (pin, F5);
  delay(x);
  noTone(pin);

  tone (pin, E5);
  delay(x);
  noTone(pin);
  tone (pin, Eb5);
  delay(x);
  noTone(pin);
  tone (pin, E5);
  delay(x * 2);
  noTone(pin);
  delay(2 * x);
  tone (pin, Gd4);
  delay(x * 2);
  noTone(pin);
  tone (pin, Cd5);
  delay(x * 4);
  noTone(pin);
  tone (pin, C5);
  delay(x * 3);
  noTone(pin);
  tone (pin, B4);
  delay(x);
  noTone(pin);

  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, A4);
  delay(x);
  noTone(pin);
  tone (pin, Bb4);
  delay(x * 2);
  noTone(pin);
  delay(2 * x);
  tone (pin, Eb4);
  delay(x * 2);
  noTone(pin);
  tone (pin, Gb4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);

  tone (pin, G4);
  delay(x * 4);
  noTone(pin);
  tone (pin, Eb4);
  delay(x * 3);
  noTone(pin);
  tone (pin, Bb4);
  delay(x);
  noTone(pin);
  tone (pin, G4);
  delay(x * 8);
  noTone(pin);
}

