#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd; // LCD instance

// ---- pins ----
const int bump_switch     = 8;
const int optical_sensor  = A0;
const int distance_sensor = A1;

const int M1_direction = 4;
const int M1_PWM       = 5;
const int M2_direction = 7;
const int M2_PWM       = 6;

// ---- optical sensor / revolution counting ----
int  threshold        = 100;   // threshold for black/white detection
int  revolutions      = 0;
bool current_reading  = false; // true when black mark seen
bool previous_reading = false;

// ---- distance sensing ----
// simple mapped “close/far” representation for now
// (you can later replace this with a calibrated cm conversion)

void setup() {
    Serial.begin(9600);
    Wire.begin();

    pinMode(bump_switch, INPUT_PULLUP);
    pinMode(M1_direction, OUTPUT);
    pinMode(M1_PWM, OUTPUT);
    pinMode(M2_direction, OUTPUT);
    pinMode(M2_PWM, OUTPUT);

    pinMode(distance_sensor, INPUT);
    pinMode(optical_sensor, INPUT);

    // ---- LCD startup ----
    lcd.lcdClear();
    lcd.lcdGoToXY(1,1);
    lcd.lcdWrite("Robot ready");

    // Wait for any LCD button before starting (15 = none pressed)
    int b = lcd.readButtons();
    while (b == 15) {
        b = lcd.readButtons();
    }
}

// ------------------ main loop ------------------
void loop() {
    // Drive forward at moderate speed
    driveForward(150);

    // Update revolutions from optical sensor
    int revs = measure_revolutions();

    // Check distance sensor
    check_distance();

    // Print revolutions to Serial for debugging
    Serial.print("Revolutions: ");
    Serial.println(revs);

    // Simple bump‑switch based avoidance
    if (digitalRead(bump_switch) == LOW) {
        avoid_obstacle();
    }

    delay(50); // small pause before next loop iteration
}

// ------------------ motion helpers ------------------

void driveForward(int s) {
    digitalWrite(M1_direction, HIGH);
    digitalWrite(M2_direction, LOW);
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void driveBackward(int s) {
    digitalWrite(M1_direction, LOW);
    digitalWrite(M2_direction, HIGH);
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void turnRight(int s) {
    // motors opposite directions to spin on the spot
    digitalWrite(M1_direction, HIGH);
    digitalWrite(M2_direction, HIGH);
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void turnLeft(int s) {
    digitalWrite(M1_direction, LOW);
    digitalWrite(M2_direction, LOW);
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void stopMotors() {
    analogWrite(M1_PWM, 0);
    analogWrite(M2_PWM, 0);
}

// ------------------ behaviours ------------------

void avoid_obstacle() {
    lcd.lcdClear();
    lcd.lcdGoToXY(1,1);
    lcd.lcdWrite("Hit!");

    // back up briefly
    driveBackward(150);
    delay(1000);

    // turn right briefly
    turnRight(150);
    delay(1000);

    stopMotors();
}

// Rising‑edge detection on the black mark
int measure_revolutions() {
    int optical_sensor_value = analogRead(optical_sensor);

    // Update current_reading based on threshold
    previous_reading = current_reading;
    if (optical_sensor_value < threshold) {
        current_reading = true;   // black mark seen
    } else {
        current_reading = false;  // white/background
    }

    // Count a revolution on the rising edge (false -> true)
    if (current_reading == true && previous_reading == false) {
        revolutions = revolutions + 1;
    }

    return revolutions;
}

// Very simple “close/far” check for the IR distance sensor
void check_distance() {
    int value = map(analogRead(distance_sensor), 0, 1023, 0, 100);

    if (value <= 20) {
        Serial.println("Close");
    } else {
        Serial.println("Far");
    }

    // small delay so we don’t spam the serial monitor too hard
    delay(1000);
}
