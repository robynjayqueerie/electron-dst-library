About
===
This is a basic class library which implements a way of determining if the local time
of a Particle device is in daylight saving time (DST)
It does this by using pointers to start and end arrays which have the following format;

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

See the example dst.ino for some use of the class
