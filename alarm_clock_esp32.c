/*
An alarm clock with ESP32 microcontroller and a servo motor. 
The user can set the alarm time and once the alarm goes off, 
the servo motor moves its "wing" for 10 seconds. While waiting for the alarm,
the user can interrupt the program and set a new alarm time.
*/

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>

// Defining constants and variables
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

static const int servoPin = 13;
Servo servo;

// NTP time server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// alarmHour and alarmMinutes variables take the hours and minutes of desired alarm time
// splitHour and splitMinutes variables take the current time from NTP server

String alarmHour;
String alarmMinutes;
String splitHour;
String splitMinutes;

void setup() {
  Serial.begin(115200);
  servo.attach(servoPin);

  // Connecting to internet
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  // Starting to retrieve time from NTP server and correcting the time zone
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void loop() {

  // Telling the user what the time is in the first place
  Serial.println();
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("The time right now is ");
  Serial.println(formattedTime);


  // Then the user can set the alarm:
  Serial.println("Please set the alarm hour:");

  while (Serial.available() == 0) {
  }

  String alarmHour = Serial.readString();
  Serial.println(alarmHour);

  Serial.println("Now set the minutes:");

  while (Serial.available() == 0) {
  }

  String alarmMinutes = Serial.readString();
  Serial.println(alarmMinutes);

  // Confirming the alarm time and informing that by pressing 
  // the q button the user can cancel the alarm and set a new time
  Serial.print("Alarm set at ");
  Serial.print(alarmHour);
  Serial.print(":");
  Serial.println(alarmMinutes);
  Serial.println("Please press q to set a new alarm time.");
  Serial.println();
  

  // Here we retrieve the current time every second (it runs but we won't print it)
  while(true) {
    timeClient.update();
    String formattedTime = timeClient.getFormattedTime();
    // Saving hours and minutes in variables
    int delimiter = formattedTime.indexOf(":");
    splitHour = formattedTime.substring(0, delimiter);
    splitMinutes = formattedTime.substring(delimiter+1, formattedTime.length()-3);

    // Once the current time (hh:mm) equals the alarm time...
    if ((splitHour == alarmHour) && (splitMinutes == alarmMinutes)) {
      ringAlarm(); // ...the alarm goes off (void ringAlarm)
      break; // afterwards the program moves back to the beginning and a new alarm can be set.
    } 
    else { // While times don't match, we wait. Q button can be pressed to quit.
      if (Serial.available() > 0) {
        char quit = Serial.read();
        if(quit=='q') {
          Serial.println("Program interrupted.");
          break;
        }        
      } else {
      Serial.print("."); // While waiting for the alarm, dots are printed on the serial monitor.
      delay(2000);
      }
    }
  }
}

// This function contains the servo motor part:
void ringAlarm() {
  int i;  
  for(i = 1; i < 6; ++i) { // Here the alarm "rings" while the variable i value is 5 (10 seconds)
    Serial.println("Wake up time!");

    // The servo turns 90 degrees to left or right every 90 milliseconds. This repeats in the loop for 10 seconds.
    for(int position = 0; position <= 90; position++) {
      servo.write(posDegrees);
      delay(10);
    }
    for(int position = 90; position >= 0; position--) {
      servo.write(position);
      delay(10);
    }
  }  
}