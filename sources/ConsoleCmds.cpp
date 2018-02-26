#include "ConsoleCmds.h"

using namespace std;

/*Dependencies listed in Clearscreen.h*/

ConsoleController::ConsoleController()
{
	cInit();

	//Only for Option 2
	/*Requesting terminal dimensions*/
	getmaxyx(stdscr, maxY, maxX);
	/*Setting dimensions for Option 2 menu window*/
	height = maxY - 2;
	width  = maxX;
	startx = (maxX - width) / 2;
	//starty = (maxY - height) / 2;
	starty = 2;
}

void ConsoleController::cInit()
{
	PD_IO::cInit();
	colourStart();
	//Attempt to make the cursor invisible
	curs_set(0);
}

void ConsoleController::colourStart()
{
	bool result;  Interface::pdColourSupport(result);
	if ( !result )
	{
		printw("This terminal doesn't support custom colours,\nusing defaults instead\n");
		PD_IO::cPause();
	}
	else
	{
		init_pair(0, COLOR_GREEN, COLOR_BLACK); //Default
		init_pair(1, COLOR_RED, COLOR_BLACK);   //Error
		init_pair(2, COLOR_BLUE, COLOR_BLACK);  //Program requests
		PD_IO::charColour(stdscr, 0);
	}
}

/*Function for printing messages, such as boot message*/
void ConsoleController::mssgprint(vector<const char *> &mssg, bool clearS)
{
	//refresh();
	if (clearS == true)
		{ clear(); }

	for (auto it : mssg)
	{
		printw(it);
		refresh();
	}
}
void ConsoleController::mssgprint(const char* mssg, bool clearS)
{
	//refresh();
	if (clearS == true)
		{ clear(); }

	printw(mssg);
	refresh();
}

/*Function for handling menu options*/
void ConsoleController::MmenuOpt(bool clear_s)
{
	/*Create the menu window*/
	WINDOW* MainMenuWin = newwin(height, width, starty, startx);

	vector<string> options = {
		"Create a new entry",
		"Edit an entry",
		"Read an entry",
		"List entries",
		"Exit"
	};

	PD_IO::Menu MainMenu(MainMenuWin, options);
	MainMenu.setKeypad();

	/*Stylise the menu borders*/
	//							we   ns
	MainMenu.setDrawBorder(true, 36, 0);

	int menuChoice;

	while(true)
	{
		clear();

		if( Interface::checkLog() )
			{ printw("WARNING: Error logging file could not be created or opened\n\n"); }

		mssgprint(MMenum, clear_s);
		flushinp();

		menuChoice = MainMenu.menuLoop();

		switch (menuChoice) {
		case 1:
			Opt0(rootDir);
			break;
		case 2:
			Opt1(rootDir);
			break;
		case 3:
			Opt2(rootDir);
			break;
		case 4:
			Opt3(rootDir);
			break;
		case 5:
			if (Opt4())
				return;
			else
				break;
		}
	}
}

/*Menu options code*/
void ConsoleController::Opt0(string rootDirect, bool clear_s)
{
	rootDirect += "\\storage\\entries";
	if( !fs::exists(rootDirect) )
		{ Interface::createDir(rootDirect); }

	string fName = "storage\\entries\\Entry " + B_IO::getDate() + f_ext;
	if (fs::exists(fName))
	{
		//If file exists, ask user if they wish to edit the file
		mssgprint(Opt0_1m, clear_s);
		int confirm;
		do
		{
			confirm = getch();
			switch (confirm)
			{
			case 89:
			case 121:
				//User wants to edit existing entry
				confirm = NULL;
				WInput = "\n\n" + B_IO::getDate("%d/%m/%Y %r") + "\n\n";
				break;
			case 78:
			case 110:
				//User does not want to edit
				return;
			}
		} while (confirm != NULL);
	}

	bool result;
	fstream page("storage\\entries\\Entry " + B_IO::getDate() + f_ext, ios::app);
	Interface::fileState(page, result, "Entry " + B_IO::getDate() + f_ext);
	if ( result )
	{
		mssgprint(Opt0m, clear_s);

		WINDOW* Opt1Win = newwin(height, width, starty, startx);

		PD_IO::TextEditor tEditor(Opt1Win, 2, 2);

		tEditor.editorLoop();

		for (auto it : tEditor.getText())
		{
			WInput += it + "\n";
		}
	}
	else
		{ PD_IO::cPause(); }
	page << WInput;
	page.close();
	WInput = "";
	curs_set(0);
}

void ConsoleController::Opt1(string rootDirect, bool clear_s)
{
	Opt3(rootDirect, clear_s);
	mssgprint(Opt2m, clear_s);

	if (files.size() == 0)
	{
		clear();
		printw("No entries where found in your chosen directory\n");
		PD_IO::cPause();
		return;
	}

	/*Create the menu window*/
	WINDOW* Opt1Win = newwin(height, width, starty, startx);

	for (auto it = files.begin(); it != files.end(); ++it)
	{
		choices.push_back(it->second.string());
	}

	PD_IO::Menu Opt1Menu(Opt1Win, choices);
	Opt1Menu.setKeypad();

	/*Stylise the menu borders*/
	//							we   ns
	Opt1Menu.setDrawBorder(true, 36, 0);

	/*Start menu loop, and store seleted filename after completion*/
	string selection = choices[Opt1Menu.menuLoop() - 1];

	/*Clean up menu data*/
	files.clear();
	choices.clear();

	selection = rootDir + "\\storage\\entries\\" + selection + f_ext;

	mssgprint(Opt0m, clear_s);
	
	wclear(Opt1Win);
	PD_IO::TEViewer tEditor(Opt1Win, 2, 2, selection);
	tEditor.editorLoop();
	
	tEditor.commitChanges();

	PD_IO::cPause();
}


void ConsoleController::Opt2(string rootDirect, bool clear_s)
{
	Opt3(rootDirect, clear_s);
	mssgprint(Opt2m, clear_s);

	if (files.size() == 0)
	{
		clear();
		printw("No entries where found in your chosen directory\n");
		PD_IO::cPause();
		return;
	}

	/*Create the menu window*/
	WINDOW* Opt2Win = newwin(height, width, starty, startx);

	for (auto it = files.begin(); it != files.end(); ++it)
		{ choices.push_back(it->second.string()); }

	PD_IO::Menu Opt2Menu(Opt2Win, choices);
	Opt2Menu.setKeypad();

	/*Stylise the menu borders*/
	//							we   ns
	Opt2Menu.setDrawBorder(true, 36, 0);

	/*Start menu loop, and store seleted filename after completion*/
	string selection = choices[Opt2Menu.menuLoop() - 1];

	/*Clean up menu data*/
	files.clear();
	choices.clear();

	PD_IO::cPause();

	selection = rootDir + "\\storage\\entries\\" + selection + f_ext;
	bool bOpen;
	string line;
	fstream chosenEntry(selection, ios::in);
	Interface::fileState(chosenEntry, bOpen, selection);

	if (bOpen)
	{
		mssgprint(Opt0m, clear_s);
		wclear(Opt2Win);
		PD_IO::TEViewer tEditor(Opt2Win, 2, 2, selection);
		wrefresh(Opt2Win);
		tEditor.editorLoop(false);
	}

	delete Opt2Win;
	clear();

	PD_IO::cPause();
}

void ConsoleController::Opt3(string rootDirect, bool clear_s)
{
	mssgprint(Opt3m, clear_s);

	printw( "Press Y key if you want to search in the default entries directory\n\n" );

	int check = getch();
	std::string path;
	if( check == 121 || check == 89 )
		{ rootDirect += "\\storage\\entries"; path = rootDirect; }
	else
	{
		char pathC[2000];
		echo();
		getnstr(pathC, 2000);
		noecho();
		path = pathC;
	}

	files.clear();
	Interface::listEntries(files, path, f_ext);

	for (auto it = files.begin(); it != files.end(); ++it)
		{ printw( "%s\n", it->second.string().c_str() ); }

	PD_IO::cPause();
}

bool ConsoleController::Opt4()
{
	mssgprint(Opt4m, true);

	int confirm;
	while (true)
	{
		confirm = getch();
		switch (confirm)
		{
		case 89:
		case 121:
			//User wants to exit
			return true;
		case 78:
		case 110:
			//User does not want to exit
			return false;
			break;
		}
	}
}


namespace Interface
{
	ConsoleController* CC = new ConsoleController;
	ErrObj* ErrStore = new ErrObj;
	HandleExcep* ExcepCode = new HandleExcep(*ErrStore, "storage\\Errors.log");

	bool checkLog()
		{ return ExcepCode->logFail; }

	void createDir(std::string dir)
	{
		ExcepCode->exceptionWall( [&] () -> void
			{ B_IO::CreateDir(dir); } );
	}

	void fileState(fstream &file, bool &result, std::string fileName)
	{
		if( file.is_open() )
			{ result = true; }
		else
		{
			file.close();
			result = false;
			fileName += " couldn't be opened or created";

			ExcepCode->exceptionWall( [&] () -> void
				{ throw fileName.c_str(); } );
		}
	}

	void pdColourSupport(bool &result)
	{
		if( !PD_IO::colourStart() )
		{
			result = false;

			ExcepCode->exceptionWall( [&] () -> void
			{ throw "This terminal doesn't support custom colours," \
				" using defaults instead"; }, INFO);
		}
		result = true;
	}

	bool listEntries(std::map<int, fs::path>& tempMap, const fs::path& path,
		std::string fileExtention, B_IO::SearchFilter searchFilter)
	{
		//ListEntries will throw an error if it fails, preventing it from reporting
		//failure by returning false. However, if it is assumed
		//that ListEntries always fails the issue is resolved; and, we can set the fail state to false when it succeeds.
		bool bfail = true;
		ExcepCode->exceptionWall( [&] () -> void
		{
			if ( B_IO::ListEntries(tempMap, path, fileExtention, searchFilter) )
				{ bfail = false; }
		} );
		if (bfail)
		{
			printw("The provided directory, or file, either doesn't exist,\n" \
				"or this program doesn't have permission to access it\n");
		}
		return bfail;
	}
}
