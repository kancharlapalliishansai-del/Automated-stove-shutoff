#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo gasValve;

const int potPin = A0;
const int buttonPin = 2;
const int buzzerPin = 11;
const int servoPin = 9;

int setMinutes = 0;
unsigned long startTime;
unsigned long durationMs;
bool isRunning = false;

void setup() {
  lcd.init();
  lcd.backlight();
  gasValve.attach(servoPin);
  gasValve.write(90); // Initial "ON" position
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  if (!isRunning) {
    // Setting Phase
    int val = analogRead(potPin);
    setMinutes = map(val, 0, 1023, 0, 60); // 0 to 60 mins
    
    lcd.setCursor(0, 0);
    lcd.print("Set Time: ");
    lcd.print(setMinutes);
    lcd.print("m  ");
    
    if (digitalRead(buttonPin) == LOW && setMinutes > 0) {
      delay(200); // Debounce
      isRunning = true;
      startTime = millis();
      durationMs = (unsigned long)setMinutes * 60000;
      lcd.clear();
    }
  } else {
    // Countdown Phase
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= durationMs) {
      // TIMER FINISHED
      gasValve.write(0); // Turn to OFF position
      digitalWrite(buzzerPin, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("STOVE OFF!     ");
      delay(5000);
      digitalWrite(buzzerPin, LOW);
      isRunning = false;
    } else {
      // Display Remaining Time
      unsigned long remaining = (durationMs - elapsed) / 1000;
      lcd.setCursor(0, 0);
      lcd.print("Cooking...");
      lcd.setCursor(0, 1);
      lcd.print("Time: ");
      lcd.print(remaining / 60);
      lcd.print("m ");
      lcd.print(remaining % 60);
      lcd.print("s    ");
    }
  }
}
