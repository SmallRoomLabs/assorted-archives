// Test the PS/2 + 16 x 4 LCD Hardware for CDC160

// include the library code:
#include <LiquidCrystal.h>
#include <PS2Keyboard.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A0,A1,A2,A3,A4,A5);
PS2Keyboard keyboard;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  // Set up keyboard
  keyboard.begin(8,3);
}

void loop() {
  if (keyboard.available()) {
    lcd.print(keyboard.read());
  }
}

