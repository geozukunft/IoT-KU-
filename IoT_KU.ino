#include <SPI.h>
#include <WiFiNINA.h>
#include <MQTTClient.h>
#include <MQTT.h>



//define settings for Wifi and MQTT Broker
#define BROKER_IP    "90.146.19.12"
#define DEV_NAME     "ardunioMKR"
#define MQTT_USER    "mqttUser"
#define MQTT_PW      "HIGHsecPW"

//Set wifi settings
const char ssid[] = "devices";
const char pass[] = "Z6Opm2ezigp8";

//Create Wifi and MQTT client 
WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

// Init of Pin Values to Sensor Data
int triggerUS = 5; //Trigger Pin Ultrasonic
int echoUS = 6; //Echo Pin Ultrasonic
int triggerLB = 4; //Trigger Pin Lightbridge
int dataFS = A0; //Analog Input of Forcesensor

// Init vars needed for sensors
int dataLB = 0; //current state of Lightbridge
int durationUS = 0; //
int distanceUS = 0;
double rawData = 0;
int percent = 0;

//Init error vars
int errorUS = 0; //Error Var for Ultrasonic 
int errorLB = 0; //Error Var for Lightbridge
int errorFS = 0; //Error Var for Forcesensor


//Init functions
int ultrasonic();

int lightbarrier();

int forcesensor();

void serialData();

void connect();

// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(triggerUS, OUTPUT);
	pinMode(echoUS, INPUT);

	pinMode(triggerLB, INPUT);
	pinMode(dataFS, INPUT);

	Serial.begin(9600);

	WiFi.begin(ssid, pass);
	client.begin(BROKER_IP, 1883, net);
	connect();


}

// Add the main program code into the continuous loop() function
void loop()
{
	errorUS = ultrasonic();
	errorLB = lightbarrier();
	errorFS = forcesensor();
	serialData();

	client.loop();
	if (!client.connected()) {
		connect();
	}
	if (millis() - lastMillis > 1000) {
		lastMillis = millis();
		client.publish("/hello", "world"); //PUBLISH TO TOPIC /hello MSG world
		client.publish("/LB", "%i", dataLB); //PUBLISH Lightbride data to /LB
		client.publish("/US", "%i", distanceUS); //PUBLISH distance data to /US
		client.publish("/FS", "%i", percent); //PUBLISH percentage of force sensor to /FS
	}

}

int ultrasonic()
{
	digitalWrite(triggerUS, LOW);
	delay(5);
	digitalWrite(triggerUS, HIGH);
	delay(10);
	digitalWrite(triggerUS, LOW);
	durationUS = pulseIn(echoUS, HIGH);
	distanceUS = (durationUS / 2) * 0.03432;

	if (distanceUS <= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int lightbarrier()
{
	dataLB = digitalRead(triggerLB);
	if (dataLB == 0)
	{
		dataLB = 1;
	}
	else if (dataLB == 1)
	{
		dataLB = 0;
	}

	return 0;
}


int forcesensor()
{
	
	rawData = (analogRead(dataFS) / 1023);
	percent = rawData * 100;

	return 0;

}

void serialData()
{

	//Serial Print of Ultrasonic Data
	Serial.print("Distanz: ");
	Serial.print(distanceUS);
	Serial.println("cm");

	//Serial Print of Lightbarrier Data
	Serial.print("Lichtschranke: ");
	Serial.print(dataLB);
	Serial.println();

	//Serial Print of Force Sensor Data
	Serial.print("Kraft Prozent: ");
	Serial.print(percent);
	Serial.println();

}

void connect() {
	Serial.print("checking wifi...");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(1000);
	}
	Serial.print("\nconnecting...");
	while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
		Serial.print(".");
		delay(1000);
	}
	Serial.println("\nconnected!");
	client.subscribe("/hello"); //Test subscribe
	client.subscribe("/LB"); //SUBSCRIBE TO TOPIC Lightbridge
	client.subscribe("/US"); //SUBSCRIBE TO TOPIC Ultrasonic sensor
	client.subscribe("/FS"); //SUBSCRIBE TO TOPIC Force
}