const int relayPin = 19;

void setup() {
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, 0);
  delay(10000);

  digitalWrite(relayPin, 1);
  delay(10000);
}
