// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       IoT_KU.ino
    Created:	11.12.2019 13:01:59
    Author:     DESKTOP-DB8RJ3G\geozu
*/

// Define User Types below here or use a .h file
//

// Define Function Prototypes that use User Types below here or use a .h file
//

// Define Functions below here or use other .ino or cpp files
//

int triggerUS = 5;
int echoUS = 6;
int triggerLB = 4;
int dataFS = A0;


int dataLB = 0;
int durationUS = 0;
int distanceUS = 0;
double rawData = 0;
int percent = 0;


int errorUS = 0;
int errorLB = 0;
int errorFS = 0;

int ultrasonic();

int lightbarrier();

int forcesensor();

void serialData();

// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(triggerUS, OUTPUT);
	pinMode(echoUS, INPUT);

	pinMode(triggerLB, INPUT);
	pinMode(dataFS, INPUT);

	Serial.begin(9600);
}

// Add the main program code into the continuous loop() function
void loop()
{
	
	errorUS = ultrasonic();
	errorLB = lightbarrier();
	errorFS = forcesensor();

	

	




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
	if (dataLB == false)
	{
		dataLB = 1;
	}
	else if (triggerLB == true)
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