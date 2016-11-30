#include <time.h>

#define secsDay 3600*24		// seconds in a day
#define secsWeek 3600*24*7	// seconds in week

/**
	DST works from two arrays a DST start and a DST end array.
	DST accepts pointers to the arrays so the actual names are not important but
	they will be of the form
	char dstStartRule[] = {1,1,10,2,1};	//	DST Start 
	char dstEndRule[] = {1,1,4,2,0};	// 	DST End
	The elements of each array are
	{Occurrence in the month, ie if it is the first Sunday of the month this is 1, 
		Day number using Particle numbering Sunday=1, 
		Month number using Particle numbering October = 10, 
		Hour past midnight 2=2AM, 
		DST offset in full hours, converted to float for Particle Time functions
			If fractions of an hour are to be used, ie 0.5 then this could be stored outside}
			of the DST array, the DST class does not use this value at any point
			rather the calling function will call Time.setDSTOffset() with the appropriate value
	}
	Notes on local time and DST time
	The routines used here assume the start and end arrays are given as local time without
		DST change. This means that in comparing local time as given by Time.local()
		with the start and end DST points, the comparison should be done without DST.
		For this reason the routine isDST() ends DST if it is implemented and restarts
		it before returning only if it was originally set. The actual setting of DST 
		is left to the user based on the results provided by these routines.
		
	What can be confusing is that the Particle function Time.format() takes into account
		whether the DST is set or not and also the timezone set. See
		https://community.particle.io/t/time-zone-and-time-local/23029/3
		This means that you need to be careful with examining the results of any
		call to these routines with Time.format()

*/
class DST{
	public:
		int getDSTSecs(char* rule,bool nextYear);
		bool isDST(char* sDST,char* eDST);
};


/**
	Returns the local time of a DST end or start point as given in
		the rule[] array provided
	nextYear set true if the date is required in the year after the current one
*/

int DST::getDSTSecs(char* rule,bool nextYear)
{
time_t secs;
struct tm myTime;
struct tm* tp;
char cnt;
char* cPtr;
	cPtr = rule;
	// set up for the 1st day of the given month this year
	myTime.tm_sec = 0;
	myTime.tm_min = 0;
	myTime.tm_hour = cPtr[3];//rule[3];
	myTime.tm_mday = 1;
	myTime.tm_mon = cPtr[2] - 1;//rule[2] - 1;
	if(nextYear)myTime.tm_year = Time.year() - 1900 + 1;
	else myTime.tm_year = Time.year() - 1900;
	tp = &myTime;
	secs = mktime(&myTime);		// seconds
	tp = localtime(&secs);
	// should now have day of the week for first of month
	secs = secs + (6 + cPtr[1] - myTime.tm_wday) * secsDay;		// seconds added to get to my target day
	tp = localtime(&secs);
	// now add on the seconds in the week to get to the nth target day of month
	for(cnt=1;cnt<cPtr[0];cnt++){
		secs = secs + secsWeek;
	}
	tp = localtime(&secs);
	return secs;
}
/**
	Routine returns true if current local time is such that DST is in effect
	Returns false otherwise
*/
bool DST::isDST(char* sDST,char* eDST)
{
int timeNow, timeDSTStart,timeDSTEnd;
bool dstSet;
	// if DST is already set then end it to compare local time without it
	dstSet = Time.isDST();
	if(dstSet)Time.endDST();
	timeNow = Time.local();
	// get DST start seconds
	timeDSTStart = getDSTSecs(sDST,false);
	// get DST end seconds for this year
	timeDSTEnd = getDSTSecs(eDST,false);
	// if the DST end time has already past then we need to get it for next year
	if(timeDSTEnd < timeNow)timeDSTEnd = getDSTSecs(eDST,true);
	if((Time.local() >= timeDSTStart) && (Time.local() <= timeDSTEnd)){
		// we are in DST country, if DST was previously set then reset it
		if(dstSet)Time.beginDST();
		return true;
	}
	return false;
}
