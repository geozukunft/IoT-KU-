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
	serialData();
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