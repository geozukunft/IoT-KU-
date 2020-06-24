#include <SPI.h> //Libary to communicate with the WiFi module
#include <WiFiNINA.h> //Wifi Library 
#include <MQTTClient.h> //Lib to create MQTT client
#include <MQTT.h> //Lib to connect to MQTT server
#include <WiFiUdp.h> //Lib for Network Transfer




//define settings for Wifi and MQTT Broker
#define BROKER_IP    "192.168.22.50"
#define DEV_NAME     "LennertSchicklgruber"
#define MQTT_USER    ""
#define MQTT_PW      ""
#define GROUPTOPIC	"SL"

//Set wifi settings
const char ssid[] = "";
const char pass[] = "";




//Create Wifi and MQTT client 
WiFiClient net;
MQTTClient client;
IPAddress ip;

//Create var for measure of last MQTT send time
unsigned long lastMillis = 0;

// Init of Pin Values to Sensor Data
int triggerUS = 5; //Trigger Pin Ultrasonic
int echoUS = 6; //Echo Pin Ultrasonic
int triggerLB = 4; //Trigger Pin Lightbridge
int dataFS = A0; //Analog Input of Forcesensor

// Init vars needed for sensors
int dataLB = 0; //Current state of Lightbridge
char dataLBstring[16] = ""; //Lightbridge String
int durationUS = 0; //Duration for ultrasonic signal 
int distanceUS = 0; //Ultrasonic distance
char distanceUSstring[16] = ""; //String for distance
double rawData = 0; //Raw data of force sensor
int percent = 0; //Percentage of force sensor
char percentstring[16] = ""; //String for percentage 

//Init error vars CURRENTLY not in USE active use
int errorUS = 0; //Error Var for Ultrasonic 
int errorLB = 0; //Error Var for Lightbridge
int errorFS = 0; //Error Var for Forcesensor


//Init functions

int ultrasonic(); //Function for measuring distance with US sensor

int lightbarrier(); //Function for LB input

int forcesensor(); //Function for getting and calculating FS data

void serialData(); //Function for sending data over serial 

void connect(); //Function for connecting to the WiFi network and the MQTT broker



// The setup() function runs once each time the micro-controller starts
void setup()
{

	//Define Inputs and Outputs for sensors
	pinMode(triggerUS, OUTPUT);
	pinMode(echoUS, INPUT);

	pinMode(triggerLB, INPUT);
	pinMode(dataFS, INPUT);
	digitalWrite(triggerLB, HIGH);


	//Open Serial Port with 9600 Baud Rate
	Serial.begin(9600);


	//Connect to wifi and connect to Broker and run connect function that makes sure that everything is connected
	
	WiFi.begin(ssid, pass);
	client.begin(BROKER_IP, 1883, net);
	connect();
	

}

// Add the main program code into the continuous loop() function
void loop()
{
	//Check if there are any errors that appear from one of the sensors; Currently no checking implemented
	errorUS = ultrasonic();
	errorLB = lightbarrier();
	errorFS = forcesensor();

	//Send serial Data
	serialData();


	//Send data over Wifi using MQTT;
	
	client.loop();
	if (!client.connected()) { //Check if connection still up
		connect();
	}
	if (millis() - lastMillis > 1000) { //Check if 1000ms have passed since the last run
		lastMillis = millis();

		client.publish("SL/hello", "world"); //PUBLISH TO TOPIC /hello MSG world
		snprintf(dataLBstring, 16, "%i", dataLB); //Convert Int to Char Array
		client.publish("SL/LB", dataLBstring); //PUBLISH Lightbride data to /LB
		snprintf(distanceUSstring, 16, "%i", distanceUS); //Convert Int to Char Array
		client.publish("SL/US", distanceUSstring); //PUBLISH distance data to /US
		snprintf(percentstring, 16, "%i", percent); //Convert Int to Char Array
		client.publish("SL/FS", percentstring); //PUBLISH percentage of force sensor to /FS
		Serial.println("Pushed MQTT");
	}
	

}


//Function for measuring distance with US sensor2
int ultrasonic()
{

	//Measure distance from Ultrasonic sensor
	digitalWrite(triggerUS, LOW);
	delay(5);
	digitalWrite(triggerUS, HIGH);
	delay(10);
	digitalWrite(triggerUS, LOW);
	durationUS = pulseIn(echoUS, HIGH);
	distanceUS = (durationUS / 2) * 0.03432;


	//If the data would be smaller than 0 an error would be generated
	if (distanceUS <= 0)
	{
		return 1; //No Errors 
	}
	else
	{
		return 0; //Error Code 0
	}

}


//Function for LB input
int lightbarrier()
{
	//Checks if something is in the way of the Lightbarrier
	dataLB = digitalRead(triggerLB);
	if (dataLB == 0)
	{
		dataLB = 1;
	}
	else if (dataLB == 1)
	{
		dataLB = 0;
	}

	//Return 0 because there is no Error checking
	return 0;
}


//Function for getting and calculating FS data
int forcesensor()
{
	//Reads the analog value and converts it to a normalized value
	rawData = (analogRead(dataFS) * 100 / 1023);
	percent = rawData;

	//Return 0 because there is no Error checking
	return 0;

}


//Sending Data over Serial 
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

	//Debug Print of Force Sensor Data
	/*
	Serial.print(analogRead(dataFS));
	Serial.println();
	rawData = (analogRead(dataFS) * 100 / 1023);
	Serial.println(rawData);
	*/

}


//Function to check the connection with WiFi, MQTT server and establish TOPICS for MQTT
//This code is still work in progress
void connect()
{	
	//Check if connected to wifi if not wait for connection
	Serial.print("checking wifi...");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(1000);
	}

	//Print IP to Serial 
	ip = WiFi.localIP();
	Serial.println(ip);
	Serial.print("\nconnecting...");

	//Check if connected to MQTT server if not wait for connection
	while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
		Serial.print(".");
		delay(1000);
	}
	Serial.println("\nconnected!");

	//Subscribe to Topics in MQTT
	//client.subscribe("/hello"); //Test subscribe
	//client.subscribe("/LB"); //SUBSCRIBE TO TOPIC Lightbridge
	//client.subscribe("/US"); //SUBSCRIBE TO TOPIC Ultrasonic sensor
	//client.subscribe("/FS"); //SUBSCRIBE TO TOPIC Force


}

