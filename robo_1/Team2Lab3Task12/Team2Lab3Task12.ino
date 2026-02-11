
#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd; // tells the arduino the parameters of the LCD display

//defining the constants
const int bump_switch = 8;
const int optical_sensor= A0;
const int distance_sensor = A1;



int threshold=100;      // sets the threshhold value for the optical sensor
int revolutions=0;  // sets initial revolutions

bool current_reading= false;  // setting whether the optical sensor sees the black dot
bool previous_reading= false;

const int M1_direction = 4;
const int M1_PWM = 5;
const int M2_direction = 7;
const int M2_PWM = 6;

int raw_value = 0;



//char buffer[16]; // creates a memory called buffer that can hold up to 16 characters

//basic setup
void setup() {
    Serial.begin(9600);
    Wire.begin();

    pinMode(bump_switch, INPUT_PULLUP);

    pinMode(M1_direction, OUTPUT);  //defining the function to change direction 
    pinMode(M1_PWM, OUTPUT);        //defining the function to change speed.  PWM-pulse width modulation (aka how fast the commutator flips)
    pinMode(M2_direction, OUTPUT);
    pinMode(M2_PWM, OUTPUT);
    pinMode(distance_sensor, INPUT);

    // ---- LCD startup ----
    lcd.lcdClear();
    lcd.lcdGoToXY(1,1);// starts writing on the LCD screen 1,1 pixels from the bottom right 
    lcd.lcdWrite("new code");

//reads buttons
    int b = lcd.readButtons();
    while(b == 15) {   // 15 = no button pressed
        b = lcd.readButtons();//keep reading to see which button is pressed
    }

}


//this loop defines what functions are activated and when
void loop() {

    driveforward(150);
   // Serial.println(analogRead(optical_sensor));

    measure_revolutions();//measures the revolutions
    check_distance();
    

    //int optical_sensor_value= analogRead(optical_sensor); // constantly reads the value of the optical sensor
    //Serial.println(analogRead(distance_sensor));
    //lcd.lcdClear();
    //lcd.lcdGoToXY(1,1);
    //Serial.println(measure_revolutions());
    //lcd.lcdWrite(measure_revolutions());
    //lcd.lcdWrite("Bump:");  

    // writes whether it is following the white line or not
 //   if (optical_sensor_value < threshold){
  //    lcd.lcdWrite("off line");
   // } 
   // else{
  //    lcd.lcdWrite("on line");
   // }
    

    

    if (digitalRead(bump_switch) == LOW) {    //backup from an obstacle
        avoid_obstacle();
    }

    delay(50);   //wait before repeating the loop

}


//defining all the functions that will run in the continuous in void loop
void driveforward(int s){

  digitalWrite(M1_direction, HIGH);
  digitalWrite(M2_direction, LOW);// both motors move forward

  analogWrite(M1_PWM, s);
  analogWrite(M2_PWM, s);//the speed of both motors will be integer s

}

void drive_backward(int s) {
    digitalWrite(M1_direction, LOW);
    digitalWrite(M2_direction, HIGH);// both motors go backward

    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);// speed of both motors = integer s
}

void turn_right(int s){
    digitalWrite(M1_direction, HIGH);
    digitalWrite(M2_direction, HIGH);// motors travel opposite directions
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void turn_left(int s){
    digitalWrite(M1_direction, LOW);
    digitalWrite(M2_direction, LOW);// motors travel opposite directions
    analogWrite(M1_PWM, s);
    analogWrite(M2_PWM, s);
}

void stopMotors() {
    analogWrite(M1_PWM, 0);
    analogWrite(M2_PWM, 0);
}

void avoid_obstacle() {
    lcd.lcdClear();
    lcd.lcdWrite("Hit!");

    drive_backward(150);
    delay(1000);

    // turn right
    turn_right(150);

    delay(1000);
}

int measure_revolutions(){
    double optical_sensor_value=analogRead(optical_sensor);

    if ( optical_sensor_value< threshold){

        previous_reading=current_reading;
        current_reading=true;

    
    } 
    else if (optical_sensor_value > threshold){
        previous_reading=current_reading;
        current_reading=false;

    }
    
    if (current_reading==true && previous_reading==false){
        revolutions=revolutions+1;
    }

    return revolutions;

}


void check_distance(){
     int value = map(analogRead(distance_sensor), 0, 1023, 0, 100);
    if(value <= 20){
        Serial.println("Close");
    }else{
        Serial.println("Far");
    }

    delay(1000);
}