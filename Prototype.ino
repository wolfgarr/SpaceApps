#include <NewPing.h>

#define SONAR_NUM     4 // Number or sensors.
#define maxDistance 400 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 50 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define threshold 60 // The distance threshold for the ultrasonic sensors

#define buzzer 11 // The piezo buzzer pin
int buzzerState = LOW; // Keeps track of the state of the piezo buzzer pin

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

const int echo[] = {2,3,4,5}; // These are the pins that are connected to the echo pins of the ultrasonic sensors
const int trig[] = {A0, A1, A2, A3}; // The pins that are connected to the trigger pins of the ultrasonic sensors
const int led[] = {6,7,8,9}; // LED output pins
const int accelerometer[] = {A4, A5, A6}; // Accelerometer pins, X Y and Z respectively

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(trig[0], echo[0], maxDistance),
  NewPing(trig[1], echo[1], maxDistance),
  NewPing(trig[2], echo[2], maxDistance),
  NewPing(trig[3], echo[3], maxDistance)
};



void setup() {
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;

  for(int i = 0; i < 4; i++)
  {
    pinMode(led[i], OUTPUT);
    digitalWrite(led[i], LOW);
  }
  for(int i = 0; i < 3; i++)
  {
    pinMode(accelerometer[i], INPUT);
  }

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      //if (i == 0 && currentSensor == SONAR_NUM - 1) 
        oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  if(abs(analogRead(accelerometer[0])-360) > 45 || abs(analogRead(accelerometer[1])-360) > 45)
  {
    Serial.println(analogRead(accelerometer[0]));
    Serial.println(analogRead(accelerometer[1]));
    /*for(int i = 0; i < 4; i++)
    {
      digitalWrite(led[i], HIGH);
    }
    delay(1000);
    for(int i = 0; i < 4; i++)
    {
      digitalWrite(led[i], LOW);
    }*/
    for(int i = 0; i < 100; i++)
    {
      buzzerState = !buzzerState;
      digitalWrite(buzzer, buzzerState);
      delay(1);
    }
  }

}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm\t");
    if(cm[i] < threshold && cm[i] != 0)
    {
      //playSound((int)(cm[i]), 100);
      //analogWrite(buzzer, 127);
      buzzerState = !buzzerState;
      digitalWrite(buzzer, buzzerState);
      digitalWrite(led[i], HIGH);
    }
    else
    {
      digitalWrite(led[i], LOW);
    }
  }
  Serial.println();
}
