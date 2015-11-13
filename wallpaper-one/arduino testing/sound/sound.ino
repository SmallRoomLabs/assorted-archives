
int frequencies[8] = { 0,209,383,592,668,877,1051,0 };

void setup() {

}

int n = 0;

void loop() {
  noTone(12);
  if (frequencies[n] != 0) tone(12,frequencies[n]);
  delay(500);
  n = (n + 1) & 7;
}
