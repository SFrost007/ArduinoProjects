const int LED_PIN = 13;
const int THRESHOLD = 400;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(4800);
}

void loop() {
  int sensorValue = analogRead(A0);
  if (sensorValue > THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
  }
  Serial.println(sensorValue);
  delay(20);
  digitalWrite(LED_PIN, LOW);
}
