#include <Arduino.h>

#define EN        8  // Enable pin

// Direction pins
#define X_DIR     5 
#define Y_DIR     6
#define Z_DIR     7

// Step pins
#define X_STP     2
#define Y_STP     3 
#define Z_STP     4 

const int ledGreen = 22;
const int ledRed = 24;
const int inputpin = 30;
const int outputpin = 31;

int delayTime = 500; // Delay between each pulse (uS)
int smallStep = 100; // Steps for small movements
int totalSteps = 450; // Total steps to move in one direction

// Function to step a single motor
void stepMotor(byte dirPin, byte stepperPin, boolean dir) {
  digitalWrite(dirPin, dir);
  digitalWrite(stepperPin, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepperPin, LOW);
  delayMicroseconds(delayTime);
}

void setup() {
  Serial.begin(9600);
  // Set motor control pins as outputs
  pinMode(X_DIR, OUTPUT); pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); pinMode(Y_STP, OUTPUT);
  pinMode(Z_DIR, OUTPUT); pinMode(Z_STP, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(outputpin, OUTPUT);
  pinMode(inputpin, INPUT);

  digitalWrite(outputpin, LOW);
  digitalWrite(inputpin, LOW);

  // Enable the motor driver
  digitalWrite(EN, LOW);

  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);
}

void loop() {
  if (digitalRead(inputpin) == HIGH) {
      digitalWrite(outputpin, HIGH);
      Serial.println("output sent");
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledGreen, LOW);
      delay(5000);
      digitalWrite(outputpin, LOW);
    // Concurrent small steps for all axes clockwise
      for (int i = 0; i < totalSteps; i++) {
      stepMotor(X_DIR, X_STP, LOW); // X, Clockwise
      stepMotor(Y_DIR, Y_STP, LOW); // Y, Clockwise
      stepMotor(Z_DIR, Z_STP, LOW); // Z, Clockwise
        delay(10); // Small delay to stabilize
      }
      delay(5000); // Delay after small steps
      for (int i = 0; i < totalSteps; i++) {
        stepMotor(X_DIR, X_STP, LOW); // X, Clockwise
        stepMotor(Y_DIR, Y_STP, LOW); // Y, Clockwise
        stepMotor(Z_DIR, Z_STP, LOW); // Z, Clockwise
        delay(10); // Small delay to stabilize
      }
      delay(5000); // Delay after small steps
      for (int i = 0; i < totalSteps; i++) {
        stepMotor(X_DIR, X_STP, LOW); // X, Clockwise
        stepMotor(Y_DIR, Y_STP, LOW); // Y, Clockwise
        stepMotor(Z_DIR, Z_STP, LOW); // Z, Clockwise
        delay(10); // Small delay to stabilize
      }
      delay(5000);
      // Concurrent total steps for all axes counterclockwise
      for (int i = 0; i < totalSteps; i++) {
        stepMotor(X_DIR, X_STP, HIGH); // X, Counterclockwise
        stepMotor(Y_DIR, Y_STP, HIGH); // Y, Counterclockwise
        stepMotor(Z_DIR, Z_STP, HIGH); // Z, Counterclockwise
        delay(10); // Small delay to stabilize
      }
      //delay(5000); // Delay after large step
      for (int i = 0; i < totalSteps; i++) {
        stepMotor(X_DIR, X_STP, HIGH); // X, Counterclockwise
        stepMotor(Y_DIR, Y_STP, HIGH); // Y, Counterclockwise
        stepMotor(Z_DIR, Z_STP, HIGH); // Z, Counterclockwise
        delay(10); // Small delay to stabilize
      }
      //delay(5000); // Delay after large step
      for (int i = 0; i < totalSteps; i++) {
        stepMotor(X_DIR, X_STP, HIGH); // X, Counterclockwise
        stepMotor(Y_DIR, Y_STP, HIGH); // Y, Counterclockwise
        stepMotor(Z_DIR, Z_STP, HIGH); // Z, Counterclockwise
        delay(10); // Small delay to stabilize
      }
      digitalWrite(ledRed, LOW);
      digitalWrite(ledGreen, HIGH);
      delay(5000); // Delay after large step
  }
}
