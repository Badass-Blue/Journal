// Journal.cpp : Defines the entry point for the console application.
//
#include "ConsoleCmds.h"
#include <iostream>
#include <time.h>
#include <string>

using namespace std;


int main()
{
	/*Start the program loop*/
	Interface::CC->mssgprint(Interface::CC->bootm, false);

	Interface::CC->MmenuOpt(false);

	/*System shutdown*/
	return 0;
}

