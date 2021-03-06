/*  The function below linearize the input values of the distance sensor GP2D12
 *  The function get the (int)value and return (double)distance in cm
 *  WARNING: irrelevant up to 11cm, unstable above 50cm
 *  
 *  Theory www.cs.uml.edu/teams-academy/uploads/Robots/Nonlinear.pdf
 *  Code by J.M.Friedman
 */

void setup() {
  Serial.begin(9600);
}

void loop() {
  double sensorVal = analogRead(0); // Read sensor value.
  Serial.println(linearIRS(sensorVal));
  delay(100);
}

double linearIRS(int sensorValue) {
  double a = 3;
  double b = 8;
  double c = 0;
  double k = 18500;
  double delta = sensorValue - c;
  double result = (k - delta) / (a * delta);
  return result;
}

