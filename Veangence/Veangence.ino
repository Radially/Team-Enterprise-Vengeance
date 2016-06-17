#define SensBR 4
#define SensFR 3
#define SensF 2
#define SensFL 1
#define SensBL 0
boolean d8_reg = 0;
boolean d9_reg = 0;
boolean d10_reg = 0;
boolean d11_reg = 0;
boolean vt_reg = 0;

const int pwmA = 3, //A2
          pwmB = 11, //B2
          dirA = 12, //A1
          dirB = 13, //B1

          BS = 28, //Backward sensor distance to wall
          FS = 12, //Forward sensor distance to wall
          VFS = 10, //Very Forward sensor distance to wall
          BSMIN=7,
          FSMAX = 25,
          FSSMART = 15,
          pin = 46,
          Speed = 29;


int const length = 14;
int a[length];
int index = 0;

void setup() {
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(46, OUTPUT);
  Serial.begin (9600);
  for (int i = 0; i <= 4; i++) {
    pinMode(i + 40, OUTPUT);
    pinMode(i + 30, INPUT);
  }
  for (int v = 0; v < length; v++) a[v] = 0;
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(7, OUTPUT);
  intialzeRec();
  sensorCalibration();
}

void loop() {                ///////////////////////////////////////////////////////////
 do {
    wait2Package();
    recivePack(a, 0);
    Serial.println(a[0]);
    delay(760);
  }
  while (a[0] == 0);  //exits on a[0]=1,2,3
  int t = 0;
  while (a[t] != 0 || t < length - 1) {
    t++;
    wait2Package();
    recivePack(a, t);
    Serial.println(a[t]);
    delay(760);
  }
  analysis(a);
  /*a[0] = 1;
  a[1] = 1;
  a[2] = 3;
  a[3] = 0;
  a[4] = 0;
  a[5] = 0;
  a[6] = 0;*/

}

///////////////////////////////////////////////////////////


void analysis (int a[], int i) {
  switch (a[i]) {
    case 1:
      do {
        forward();
      } while (readSens(SensBR) < BS);
      turnR();
      break;
    case 2:
      do {
        forward();
      }
      while (readSens(SensBR) < BS && readSens(SensBL) < BS);
      forward();
      delay(2000);
      break;
    case 3:
      do {
        forward();
      } while (readSens(SensBL) < BS);
      turnL();
      break;
    default:
      stop();
      break;
  }
}

void analysis (int a[]) {
  int i = 0;
  while (i < length) {
    analysis (a , i);
    i++;
  }
  while (digitalRead(47) == LOW) {
    forward();
    delay(80);
  }
  stop();
  musicTime();
}
void turnRU() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, Speed+4);
  delay(800);  //1000 for full battary
  while (readSens(SensFL) < FSSMART  ||  readSens(SensF) < BS) {
    if (readSens(SensFR) < FSMAX) break;
    tone (pin, 450);
    delay(70);
    noTone(pin);
  }
  forwardOld();
  delay(1000);  //changed to 1000, need to be 1200
  forward();
}
void turnLU() {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed+3);
  analogWrite (pwmB, 0);
  delay(800);    //950 for full battary
  while (readSens(SensFR) < FSSMART || readSens(SensF) < BS) {
    if (readSens(SensFL) < FSMAX) break;
    tone (pin, 450);
    delay(70);
    noTone(pin);
  }
  forwardOld();
  delay(1000);   //changed to 1000, need to be 1200
  forward();
}

void turnR() {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, Speed + 4);
  delay(900);
  forwardOld();
  delay(1300);
}
void turnL() {
  digitalWrite(7, HIGH);
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed + 3);
  analogWrite (pwmB, 0);
  delay(900);
  digitalWrite(7, LOW);
  forwardOld();
  delay(1300);
}
void turnB() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, Speed);
  delay(600);
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed + 8);
  analogWrite (pwmB, Speed + 8);
  delay(550);
}

void stop() {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);
  analogWrite (pwmA, 0);
  analogWrite (pwmB, 0);
}

void forward() {
  forwardOld();
  delay(140);    //140 for full battary
  if (readSens(SensFR) < FS || readSens(SensBR) < BSMIN ) {
    fixR();
    delay(180);  //220 for full battary
  }
  else if (readSens(SensFL) < FS || readSens(SensBL) < BSMIN ) {
    fixL();
    delay(180);   //220 for full battary
  }
}

void fixL() {                   // turns to the right
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 15);
  analogWrite (pwmB, 37);
}
void fixR() {                   // turns to the left
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, 33);
  analogWrite (pwmB, 15);
}
void forwardOld()
{
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);
  analogWrite (pwmA, Speed);
  analogWrite (pwmB, Speed+2);
  
}

/////////////////////////////////////   Calibrations    /////////////////////////////////////////////
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

/////////////////////////////////////   communication    /////////////////////////////////////////////
void intialzeRec() {
  pinMode(2, INPUT);
  pinMode(7, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
}

void wait2Package() {
  //digitalWrite(13, LOW);
  do {
    delay(250);
    vt_reg = digitalRead(6);
    //digitalWrite(13, HIGH);
  }
  while (vt_reg == 0);
}

int recivePack(int a[], int index) {
  int x = 0;
  if (digitalRead(2) == HIGH) x = 2;
  if (digitalRead(4) == HIGH) x++;
  a[index] = x;
}

/*int recivePack(int a[], int index) {  //2 digits in pack
  d8_reg = digitalRead(2);
  d9_reg = digitalRead(4);
  d10_reg = digitalRead(5);
  d11_reg = digitalRead(6);


  a[index] = d8_reg * 2 + d9_reg;
  a[index + 1] = d10_reg * 2 + d11_reg;
  return index + 2;
}*/

int readSens(int s) {
  digitalWrite((s + 40), LOW);
  delayMicroseconds(2);
  digitalWrite((s + 40), HIGH);
  delayMicroseconds(10);
  digitalWrite((s + 40), HIGH);
  int distance = pulseIn((s + 30), HIGH);
  if (distance > 1914 || distance < 0) return 33;
  distance = distance / 58;
  return distance;
}

/*void optimization(int a[]) {
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
}*/


void musicTime() {
  int x = 50 ; //represents the duration of a 1/16 note;
  int B3 = 246, C4 = 261, D4 = 293, E4 = 329, Fs4 = 369, G4 = 392, B4 = 494, C5 = 523, Cs5 = 44, Ds5 = 622, E5 = 659, Fs5 = 740, G5 = 784, A5 = 880, As5 = 932, B5 = 988, C6 = 1046, Cs6 = 1109, D6 = 1175, Eb6 = 1245, E6 = 1319, Fs6 = 1480, G6 = 1568, A6 = 1760, B6 = 1976;
  B5 = 988, C6 = 1046, Cs6 = 1109, D6 = 1175, Eb6 = 1245, E6 = 1319, Fs6 = 1480, G6 = 1568, A6 = 1760, B6 = 1976;
  tone(pin, B3);
  delay (12 * x);
  noTone (pin);
  tone(pin, C4);
  delay (4 * x);
  noTone (pin);
  delay(16 * x);
  tone(pin, B3);
  delay (12 * x);
  noTone (pin);
  tone(pin, C4);
  delay (4 * x);
  noTone (pin);
  delay(16 * x);
  tone(pin, B3);
  delay (8 * x);
  noTone (pin);
  tone(pin, C4);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  tone(pin, B3);
  delay (8 * x);
  noTone (pin);
  tone(pin, C4);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, C5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, Cs5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, E5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, A5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B4);
  delay (4 * x);
  noTone (pin);
  tone(pin, As5);
  delay (2 * x);
  noTone (pin);
  delay(2 * x);
  tone(pin, B5);
  delay (8 * x);
  noTone (pin);
  tone(pin, C6);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  tone(pin, B5);
  delay (8 * x);
  noTone (pin);
  tone(pin, C6);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  tone(pin, B5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Cs6);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  tone(pin, B5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Eb6);
  delay (4 * x);
  noTone (pin);
  delay(4 * x);
  delay(8 * x);
  tone(pin, Ds5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (32 * x);
  noTone (pin);
  for (int i = 0; i < 2; i++) {
    tone(pin, E4);
    delay (16 * x);
    noTone (pin);
    tone(pin, Fs4);
    delay (8 * x);
    noTone (pin);
    tone(pin, G4);
    delay (8 * x);
    noTone (pin);
    tone(pin, Fs4);
    delay (12 * x);
    noTone (pin);
    tone(pin, E4);
    delay (4 * x);
    noTone (pin);
    tone(pin, E4);
    delay (16 * x);
    noTone (pin);
    tone(pin, E4);
    delay (16 * x);
    noTone (pin);
    tone(pin, D4);
    delay (8 * x);
    noTone (pin);
    tone(pin, B3);
    delay (4 * x);
    noTone (pin);
    tone(pin, D4);
    delay (4 * x);
    noTone (pin);
    tone(pin, E4);
    delay (24 * x);
    noTone (pin);
    tone(pin, E4);
    delay (8 * x);
    noTone (pin);
    tone(pin, E4);
    delay (16 * x);
    noTone (pin);
    tone(pin, Fs4);
    delay (8 * x);
    noTone (pin);
    tone(pin, G4);
    delay (8 * x);
    noTone (pin);
    tone(pin, Fs4);
    delay (12 * x);
    noTone (pin);
    tone(pin, E4);
    delay (4 * x);
    noTone (pin);
    tone(pin, E4);
    delay (16 * x);
    noTone (pin);
    tone(pin, E4);
    delay (8 * x);
    noTone (pin);
    tone(pin, G4);
    delay (3 * x);
    noTone (pin);
    tone(pin, E4);
    delay (3 * x);
    noTone (pin);
    tone(pin, G4);
    delay (2 * x);
    noTone (pin);
    tone(pin, B4);
    delay (8 * x);
    noTone (pin);
    tone(pin, B3);
    delay (8 * x);
    noTone (pin);
    tone(pin, E4);
    delay (24 * x);
    noTone (pin);
    delay(8 * x);
  }
  for (int i = 0; i < 2; i++) {
    tone(pin, B5);
    delay (16 * x);
    noTone (pin);
    tone(pin, C6);
    delay (8 * x);
    noTone (pin);
    tone(pin, D6);
    delay (8 * x);
    noTone (pin);
    tone(pin, C6);
    delay (12 * x);
    noTone (pin);
    tone(pin, B5);
    delay (4 * x);
    noTone (pin);
    tone(pin, B5);
    delay (16 * x);
    noTone (pin);
    tone(pin, B5);
    delay (16 * x);
    noTone (pin);
    tone(pin, A5);
    delay (8 * x);
    noTone (pin);
    tone(pin, G5);
    delay (4 * x);
    noTone (pin);
    tone(pin, A5);
    delay (4 * x);
    noTone (pin);
    tone(pin, B5);
    delay (24 * x);
    noTone (pin);
    tone(pin, B5);
    delay (8 * x);
    noTone (pin);
  }
  tone(pin, E6);
  delay (16 * x);
  noTone (pin);
  tone(pin, Fs6);
  delay (8 * x);
  noTone (pin);
  tone(pin, G6);
  delay (8 * x);
  noTone (pin);
  tone(pin, Fs6);
  delay (12 * x);
  noTone (pin);
  tone(pin, E6);
  delay (4 * x);
  noTone (pin);
  tone(pin, E6);
  delay (16 * x);
  noTone (pin);
  tone(pin, E6);
  delay (16 * x);
  noTone (pin);
  tone(pin, D6);
  delay (8 * x);
  noTone (pin);
  tone(pin, B5);
  delay (4 * x);
  noTone (pin);
  tone(pin, D6);
  delay (4 * x);
  noTone (pin);
  tone(pin, E6);
  delay (24 * x);
  noTone (pin);
  tone(pin, E6);
  delay (8 * x);
  noTone (pin);
  tone(pin, E6);
  delay (16 * x);
  noTone (pin);
  tone(pin, Fs6);
  delay (8 * x);
  noTone (pin);
  tone(pin, G6);
  delay (8 * x);
  noTone (pin);
  tone(pin, A6);
  delay (12 * x);
  noTone (pin);
  tone(pin, G6);
  delay (2 * x);
  noTone (pin);
  tone(pin, A6);
  delay (2 * x);
  noTone (pin);
  tone(pin, B6);
  delay (16 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  tone(pin, E5);
  delay (12 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (8 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  tone(pin, E5);
  delay (12 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (8 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (8 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  tone(pin, E5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (4 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  tone(pin, E5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (4 * x);
  noTone (pin);
  tone(pin, G5);
  delay (2 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (2 * x);
  noTone (pin);
  tone(pin, E5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Fs5);
  delay (4 * x);
  noTone (pin);
  tone(pin, Ds5);
  delay (4 * x);
  noTone (pin);
  tone(pin, E5);
  delay (32 * x);
  noTone (pin);
}



