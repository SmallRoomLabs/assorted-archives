const int buttonPin = A0;      // Connect via switch to GND
const int ledPin =  A2;       // Connect via 220R to GND
const int speakerPin = A1;    // Connect via Piezo Buzzer to GND

int buttonState = 0;         

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
    tone(speakerPin,440);
  }
  else {
    digitalWrite(ledPin, LOW);
    noTone(speakerPin);
  }
}
