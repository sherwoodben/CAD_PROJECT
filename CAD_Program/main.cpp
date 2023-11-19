//main.cpp

//include CAD_APP as it's the "heart" of the program
#include "CAD_APP.h"


//Pointer for the CAD_APP:
CAD_APP* application;


//main loop
int main()
{
	//create an instance of the application
	application = new CAD_APP();

	//check that the application
	//instantiated correctly:
	if (!application->InitializeApp())
	{
		//we failed
		return -1;
	}

	//while the application should be open
	while (!application->ShouldClose())
	{
		//do the loop:
		application->MainLoop();

	}

	//we're done, release memory accordingly
	delete application;

	//default return code
	return 0;
}