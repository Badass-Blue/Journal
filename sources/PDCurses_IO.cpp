#include "PDCurses_IO.h"

void PD_IO::charColour(WINDOW* win, int colourPair, int colourType)
{
	switch(colourType)
	{
	case DEFAULT:
		wbkgdset(win, COLOR_PAIR(colourPair));
		refresh();
		break;
	case OVERRIDE:
		wattrset(win, COLOR_PAIR(colourPair));
		refresh();
		break;
	case ON:
		wattron(win, COLOR_PAIR(colourPair));
		refresh();
		break;
	case OFF:
		wattroff(win, COLOR_PAIR(colourPair));
		refresh();
		break;
	}
}

void PD_IO::cPause(WINDOW* win, const char* message)
{
	wprintw(win, message);
	flushinp();
	wgetch(win);
}

void PD_IO::cInit(WINDOW* window, bool bKeypad)
{
	initscr();
	noecho();
	if( bKeypad )
		keypad(window, true);
}

bool PD_IO::colourStart()
{
	if ( !has_colors() || !can_change_color() )
		return false;

	start_color();
	return true;
}



PD_IO::MenuBase::MenuBase(WINDOW* menu_win, std::vector<std::string> choices)
{
	this->choices = choices;
	numChoices = choices.size();
	this->menuWin = menu_win;
	scrollok(this->menuWin, true);
}

PD_IO::Menu::Menu(WINDOW* menu_win, std::vector<std::string> choices) \
	: MenuBase(menu_win, choices)
{
	printLabelVertical = true;
	drawBorder = true;
	bordersWE, bordersNS = 0;
}

int PD_IO::Menu::menuLoop(int startChoice)
{
	int highlight = startChoice;
	if (drawBorder)
		box(menuWin, bordersWE, bordersNS);
	menuHighlight(menuWin, highlight);

	while (true)
	{
		int choice = 0;
		int input = wgetch(menuWin);
		switch (input)
		{
		case KEY_UP:
			if (highlight == 1)
				highlight = numChoices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if (highlight == numChoices)
				highlight = 1;
			else
				++highlight;
			break;
		case 10:
			choice = highlight;
			break;
		default:
			break;
		}
		menuHighlight(menuWin, highlight);

		if (choice != 0)
		{
			//User made a decision, exit
			return choice;
		}
	}
}

inline void PD_IO::Menu::printHorizontal(int& x, std::vector<std::string> array, int currentIndex)
	{ x += array[currentIndex].size(); }
inline void PD_IO::Menu::printVertical(int& y)
	{ ++y; }
inline void PD_IO::Menu::setPrintOrientation(bool orientation)
	{ printLabelVertical = orientation; }
void PD_IO::Menu::setDrawBorder(bool draw, int bordersWE, int bordersNS)
{
	drawBorder = draw;
	this->bordersWE  = bordersWE;
	this->bordersNS = bordersNS;
}
void PD_IO::Menu::menuHighlight(WINDOW *menu_win, int highlight, int xLable, int yLable)
{
	int x, y;

	//Postioning of choice lables in window
	x = xLable;
	y = yLable;

	for (int i = 0; i < numChoices; ++i)
	{
		//Highlight the present choice
		if (highlight == i + 1)
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i].c_str());
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i].c_str());
		//Move to next line, or column, for next label
		if (printLabelVertical)
			printVertical(y);
		else
			printHorizontal(x, choices, i);
	}
	wrefresh(menu_win);
}



PD_IO::TextEditorBase::TextEditorBase(WINDOW* textWin, int yPadding, int xPadding) : textWin(textWin), yPadding(yPadding),
	xPadding(xPadding)
{
	maxX = getmaxx(textWin);
	maxY = getmaxy(textWin);

	linesPerPage = maxY - (yPadding +1);
	charsPerLine = maxX - (xPadding * 2);

	pageNum = 0;

	scrollok(textWin, true);
	noecho();
	cbreak();
	curs_set(1);
	keypad(textWin, true);
	wmove(textWin, yPadding, xPadding);
	wrefresh(textWin);
}

PD_IO::TextEditor::TextEditor(WINDOW* textWin, int yPadding, int xPadding) \
	: TextEditorBase(textWin, yPadding, xPadding) { }

void PD_IO::TextEditor::printPage(bool up)
{
	wclear(textWin);
	//Find where the first line is of the current page
	int printLine = pageNum * linesPerPage;
	//										Subtract one as size is not zero initialised
	for (int lineNumber = 0; !(lineNumber > (textBuffer.size() -1)) && !(printLine > (textBuffer.size() - 1)); ++lineNumber, ++printLine)
	{
		if(!(lineNumber > linesPerPage))
		{
			wmove(textWin, lineNumber + yPadding, xPadding);
			waddstr(textWin, textBuffer[printLine].c_str());
			wrefresh(textWin);
		}
	}

	if (!up)
	{
		wmove(textWin, yPadding, linePosition[1] + xPadding);
		wrefresh(textWin);
	}
	else
	{
		wmove(textWin, (linePosition[0] - (linesPerPage * pageNum)) + yPadding, linePosition[1] + xPadding);
		wrefresh(textWin);
	}
}


void PD_IO::TextEditor::addCharacter(char character)
{
	auto normalAdd = [&]() ->void {
		textBuffer[linePosition[0]].insert(linePosition[1], &character);

		/*Update cursor position and screen*/
		linePosition[1] += 1;
		updateScreen(false, character);

		/*Check if adding character off screen*/
		isOffScreen();
	};
	auto normalAddStr = [&](std::string s) ->void {
		textBuffer[linePosition[0]].insert(linePosition[1], s);

		/*Update cursor position and screen*/
		linePosition[1] += 1;
		updateScreen(false, character);

		/*Check if adding character off screen*/
		isOffScreen();
	};

	auto newLineAdd = [&]() ->void {
		linePosition[1] = 0;
		/*Check if adding character off screen*/
		isOffScreen();
		
		textBuffer.push_back("");
		//Moves down by default, and increments linePosition[0]
		moveCursorUpDown();
	};

	/* 
	 * Check if line will be full when inserting character. Add one to size
	 * to get future size
	 */
	if ((textBuffer[linePosition[0]].size() + 1) >= charsPerLine)
	{	
		std::string lastChar;
		while(true)
		{
			if (character == 10)
				break;
			
			/*Cut last character on current line*/
			lastChar = textBuffer[linePosition[0]].substr(textBuffer[linePosition[0]].size() -1, 1);
			textBuffer[linePosition[0]].erase\
				(textBuffer[linePosition[0]].begin() + (textBuffer[linePosition[0]].size() -1));
			if(linePosition[1] > (textBuffer[linePosition[0]].size() -1))
				linePosition[1] -= 1;

			/*Check if a line below exists. If so, insert character and paste last char on the line below*/
			if ( !((linePosition[0] +1) > (textBuffer.size() -1) ) )
			{
				normalAdd();
				
				linePosition[0] += 1;
				linePosition[1] = 0;
				normalAddStr(lastChar);
				
				/*Ensure the line is not larger than allowed after operation*/
				if (textBuffer[linePosition[0]].size() < charsPerLine)
				{
					/*If not, job done*/
					return;
				}
				else
					character = NULL;
			}
			else
			{
				/*If not, create a new line. Paste last char on the line below*/
				normalAdd();

				newLineAdd();

				normalAddStr(lastChar);
				return;
			}
		}
	}

	if (character == 10)
	{
		/*If character is carriage return*/

		newLineAdd();
		return;
	}
	normalAdd();
}

void PD_IO::TextEditor::deleteCharacter()
{
	auto normalDel = [&]() ->void {
		textBuffer[linePosition[0]].erase(textBuffer[linePosition[0]].begin() + linePosition[1]);
	};
	
	
	//linePosition[1] = position of cursor
	/*Check if deleting last character on line*/
	if (linePosition[1] == 0)
	{
		/*Check if deleting empty line*/
		if (textBuffer[linePosition[0]].size() == 0)
		{
			/*Check if not deleting last character in existance*/
			if (linePosition[0] == 0)
				return;
			else
			{
				/*Remove the line and update cursor position and tracker*/
				textBuffer.pop_back();
				linePosition[1] = 0;
				linePosition[0] -= 1;

				wmove(textWin, linePosition[0] + yPadding, linePosition[1] + xPadding);
				wrefresh(textWin);
				return;
			}
		}
		else
		{
			/*Lastly, if other characters do exist after the first character,
			 * delete the first character using default method*/
			normalDel();
		}
	}
	else
	{
		//since cursor will delete char behind it, we must -1 to get char position
		linePosition[1] -= 1;
		normalDel();
	}

	updateScreen(true, 0);
}

void PD_IO::TextEditor::updateScreen(bool bDelete, char character)
{	
	if (bDelete)
		mvwdelch(textWin, (linePosition[0] - (linesPerPage * pageNum)) + yPadding, linePosition[1] + xPadding);
	else
	{
		//This line won't work as it can't insert characters
		//mvwaddch(textWin, linePosition[0] + yPadding, (linePosition[1] - 1) + xPadding, character);

		/*Clear and re-print line*/
		wmove(textWin, (linePosition[0] - (linesPerPage * pageNum)) +yPadding, xPadding);
		wclrtoeol(textWin);
		waddstr(textWin, textBuffer[linePosition[0]].c_str());
		/*Move cursor back to original position*/
		wmove(textWin, (linePosition[0] - (linesPerPage * pageNum)) + yPadding, linePosition[1] +xPadding);
	}

	/*Print the updated string and move cursor to original position*/
	wrefresh(textWin);
}

bool PD_IO::TextEditor::isOffScreen()
{
	/*
	 * (? : ) comparison exists as pages after the first cannot have >=.
	 * If so, a line will be registered on two pages causing undefined behaviour
	 */
	if ((pageNum > 0 ? linePosition[0] > (linesPerPage *pageNum) : \
		linePosition[0] >= (linesPerPage *pageNum)) && (linePosition[0] <= (linesPerPage *(pageNum + 1))))
		return false;
	
	if (linePosition[0] > (linesPerPage * pageNum))
	{
		++pageNum;
		printPage(false);
	}
	else
	{
		--pageNum;
		printPage();
	}
	return true;
}

void PD_IO::TextEditor::moveCursorUpDown(bool down)
{
	if (!down)
	{
		/*Check if already at the top of page*/
		if (linePosition[0] == 0)
			{ return; }

		/*Move the cursor up a line and update data structure*/
		linePosition[0] -= 1;
	}
	else
	{
		/*Check if already at the bottom of page*//*
		if (linePosition[0] == linesPerPage)
			{ return; }*/
		/*Check if there are any lines to move down to*/
		if ((linePosition[0] + 1) > (textBuffer.size() - 1))
			return;

		/*Move the cursor down a line and update data structure*/
		linePosition[0] += 1;
	}

	/*Check if moving cursor off screen. If so, move to page in cursor direction*/
	if (isOffScreen())
	{
		return;
	}

	linePosition[1] = 0;
	wmove(textWin, (linePosition[0] - (linesPerPage * pageNum)) + yPadding, linePosition[1] +xPadding);

	wrefresh(textWin);
}

void PD_IO::TextEditor::moveCursorLeftRight(bool left)
{
	if (left)
	{
		if(linePosition[1] != 0)
			linePosition[1] -= 1;
		else if (linePosition[0] == 0)
		{
			/*Check if a line above exists, if x = 0*/
			return;
		}
		else
		{
			/*If so, move up a line*/
			linePosition[0] -= 1;
			linePosition[1] = 0;
			isOffScreen();
		}
	}
	else
	{
		/*Check if character exists to move to*/
		if ((linePosition[1] + 1) > textBuffer[linePosition[0]].size())
		{
			/*
			 *If character doesn't exist on this line, check if a line below exists
			 * to move to
			 */
			//-1 from size so it's a zero initialised number
			if ( !( (linePosition[0] + 1) > (textBuffer.size() -1) ) )
			{
				linePosition[0] += 1;
				linePosition[1] = 0;
				isOffScreen();
			}
			else
				return;
		}

		else if (linePosition[1] != charsPerLine)
			linePosition[1] += 1;
	}
	
	wmove(textWin, (linePosition[0] - (linesPerPage * pageNum)) + yPadding, linePosition[1] +xPadding);
	wrefresh(textWin);
}

void PD_IO::TextEditor::editorLoop(bool enableEdit)
{
	int key;

	//Loop while the ESC key hasn't been pressed
	while (true)
	{
		key = wgetch(textWin);
		//Accept character if an error or the ESC key did not occur
		switch (key)
		{
		case 8:
			//Backspace
			if(enableEdit)
				deleteCharacter();
			break;
		case KEY_UP:
			moveCursorUpDown(false);
			break;
		case KEY_DOWN:
			moveCursorUpDown();
			break;
		case KEY_LEFT:
			moveCursorLeftRight();
			break;
		case KEY_RIGHT:
			moveCursorLeftRight(false);
			break;
		case 27:
		case ERR:
			//ESC key or error
			flushinp();
			return;
		default:
			if (enableEdit)
				addCharacter(key);
			break;
		}
	}
}



PD_IO::TEViewer::TEViewer(WINDOW* textWin, int yPadding, int xPadding, std::string inputFile) : inputFile(inputFile),
	TextEditor(textWin, yPadding, xPadding),
	TextEditorBase(textWin, yPadding, xPadding)
{
	parseINFile();
	printPage();
}

void PD_IO::TEViewer::parseINFile()
{
	std::fstream inputText(inputFile, std::ios::in);
	/*Read file and remove \n*/
	if (inputText.is_open())
	{
		std::string line;
		while (getline(inputText, line))
		{
			if (line.size() > charsPerLine)
			{
				std::string temp = line.substr(charsPerLine, line.size() - charsPerLine);
				line.erase(charsPerLine);

				textBuffer.push_back(line);
				textBuffer.push_back(temp);
				continue;
			}
			textBuffer.push_back(line);
		}
	}

	inputText.close();
}

void PD_IO::TEViewer::commitChanges()
{
	std::fstream output(inputFile, std::ios::trunc);
	
	std::string line;
	for (auto it : textBuffer)
	{
		line += it + "\n";
	}
	if (output.is_open())
	{
		output << line;
	}
}