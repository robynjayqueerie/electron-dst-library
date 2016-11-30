/*
	See notes on Time.format()
	https://community.particle.io/t/time-zone-and-time-local/23029/3

*/
#include "dst.h"

//	I will determine when I want to go online
SYSTEM_MODE(MANUAL);

//	will be using my own SIM card
STARTUP(cellular_credentials_set("mdata.net.au", "", "", NULL));

SYSTEM_THREAD(ENABLED);

int timeNow, timeDSTStart,timeDSTEnd;
float timezone = 10.0;
char dstStartRule[] = {1,1,10,2,1};	//	DST Start {1st Occurrence, Sunday, October, 2AM, DST offset}
char dstEndRule[] = {1,1,4,2,0};	// 	DST End

DST dst;

void setup()
{
    Serial.begin(9600);
	delay(4000);
	Particle.connect();
	while(!Particle.connected())delay(1000);
	Serial.println("starting dst program");
	delay(5000);
}


void loop()
{
	Serial.printlnf("Device timezone is %f",timezone);
	Time.zone(timezone);
	Serial.print("Current time is : ");
	Serial.println(Time.format(Time.now(),"%d/%m/%Y,%H:%M:%S"));
	timeNow = Time.now();
	/* While I am checking for DST I need to turn off DST or it
			will affect the calculations
	*/
	Time.endDST();
	// get DST start seconds
	timeDSTStart = dst.getDSTSecs(dstStartRule,0);
	Serial.printlnf("DST Start %i",timeDSTStart);
	Serial.println(Time.format(timeDSTStart-timezone*3600,"%d/%m/%Y,%H:%M:%S"));
	// get DST end seconds
	timeDSTEnd = dst.getDSTSecs(dstEndRule,1);
	Serial.printlnf("DST End %i",timeDSTEnd);
	Serial.println(Time.format(timeDSTEnd-timezone*3600,"%d/%m/%Y,%H:%M:%S"));
	if((Time.local() >= timeDSTStart) && (Time.local() <= timeDSTEnd)){
		// DST is in effect
		Serial.println("DST is in effect");
		Time.setDSTOffset((float)dstStartRule[4]);
		Time.beginDST();
	}
	else{
		Serial.println("DST has finished");
		Time.endDST();
	}
	Serial.print("Current time is : ");
	Serial.println(Time.format(Time.now(),"%d/%m/%Y,%H:%M:%S"));
	if(dst.isDST(dstStartRule,dstEndRule)){
		Serial.println("DST returned that DST was in effect");
		Time.setDSTOffset((float)dstStartRule[4]);
		Time.beginDST();
		Serial.print("Current time is : ");
		Serial.println(Time.format(Time.now(),"%d/%m/%Y,%H:%M:%S"));
	}
	delay(60000);
}
