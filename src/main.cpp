#include <Arduino.h>

#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include "DHT.h"
#include <Servo.h>

#define lamp 10
#define dcMotor 9

#define DHTPIN 3      // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define servoPin 4
Servo servo1;

const int rainSensor = A0;
// lowest and highest RAIN sensor readings:
const int rainMin = 0;    // sensor minimum
const int rainMax = 1024; // sensor maximum

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "134K1A5E-siqFcnMZucBbddzFvv3L5IC";  //f411reesp
char auth[] = "tSLCLqAUk1lYC10Y3Ui5FBpoIcKehMgi"; //asus_f411
char serverip[] = "192.168.1.102";

// Your WiFi credentials.
char ssid[] = "MiNE_1";
char pass[] = "abcd1234";

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 8); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);
BlynkTimer timer;

BLYNK_WRITE(V1)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    // process received value
    if (pinValue == 1)
    {
        digitalWrite(D7, HIGH);
    }
    else
    {
        digitalWrite(D7, LOW);
    }
}

void myTimerEvent()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    // Wait a few seconds between measurements.
    delay(2000);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Compute heat index
    // Must send in temp in Fahrenheit!
    float hi = dht.computeHeatIndex(f, h);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hi);
    Serial.println(" *F");
    // Blynk.virtualWrite(V5, millis() / 1000);
    Blynk.virtualWrite(V6, String(t));

    if (t > 28)
    {
        Serial.println("It's hot");
        digitalWrite(dcMotor, HIGH);
        digitalWrite(lamp, HIGH);
    }
    else
    {
        Serial.println("It's cold");
        digitalWrite(dcMotor, LOW);
        digitalWrite(lamp, LOW);
    }

    // read the RAIN sensor on analog A0:
    int rainRead = analogRead(rainSensor);
    // map the sensor range (four options):
    int rainRange = map(rainRead, rainMin, rainMax, 0, 3);

    switch (rainRange)
    {
    case 0:
        Serial.println("Thunderstorm!");
        // Make servo go to 0 degrees
        servo1.write(0);
        delay(1000);
        break;

    case 1:
        Serial.println("Raining...");
        // Make servo go to 0 degrees
        servo1.write(0);
        delay(1000);
        break;

    case 2:
        Serial.println("Rain Warning");
        // Make servo go to 0 degrees
        servo1.write(0);
        delay(1000);
        break;
    default:
        Serial.println("Sunshine!");
        servo1.write(90);
        delay(1000);
        break;
    }
}

void setup()
{
    // Debug console
    Serial.begin(9600);
    pinMode(lamp, OUTPUT);    //lamp(led)
    pinMode(dcMotor, OUTPUT); //dcmotor
    delay(10);
    dht.begin();

    // We need to attach the servo to the used pin number
    servo1.attach(servoPin);

    // Set ESP8266 baud rate
    EspSerial.begin(ESP8266_BAUD);
    delay(10);

    Blynk.begin(auth, wifi, ssid, pass, serverip, 8080);

    // Setup a function to be called every second
    timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
}