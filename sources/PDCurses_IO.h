#ifndef PDCURSES_IO_H
#define PDCURSES_IO_H

#include <curses.h>
#include <string>
#include <vector>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

//For switch colour attributes
#define DEFAULT  0
#define OVERRIDE 1
#define ON       2
#define OFF      3
#define PAUSE_MESSAGE ("Smash any key to continue :{)")

namespace PD_IO
{
	void charColour(WINDOW* win, int colourPair, int colourType = DEFAULT);

	void cPause(WINDOW* win = stdscr, const char* message = PAUSE_MESSAGE);

	void cInit(WINDOW* window = stdscr, bool bKeypad = false);

	bool colourStart();



	class MenuBase
	{
	protected:
		WINDOW* menuWin;
		std::vector<std::string> choices;
		int numChoices;

		//Only to be used by menuLoop
		virtual void menuHighlight(WINDOW *menu_win, int highlight, int xLable=2, int yLable=2) = 0;
	public:
		MenuBase(WINDOW* menu_win, std::vector<std::string> choices);

		//User must interact with this
		virtual int menuLoop(int startChoice=1) = 0;
		
		//Getters and setters allow for customisation
		//Setters
		virtual void setKeypad(bool state=true) { keypad(this->menuWin, state); }
	};

	class Menu : virtual public MenuBase
	{
	private:
		//Only to be used by menuHighlight()
		void printHorizontal(int& x, std::vector<std::string> array, int currentIndex);
		void printVertical(int& y);
		bool printLabelVertical;
		bool drawBorder;
		int bordersWE, bordersNS;

		virtual void menuHighlight(WINDOW *menu_win, int highlight, int xLable=2, int yLable=2);
	public:
		Menu(WINDOW* menu_win, std::vector<std::string> choices);

		virtual int menuLoop(int startChoice=1);

		//Setters
		void setPrintOrientation(bool orientation);
		void setDrawBorder(bool draw=true, int bordersWE=0, int bordersNS=0);
	};


	//Window for editing text
	class TextEditorBase
	{
	protected:
		WINDOW* textWin;

		virtual void addCharacter(char character) = 0;
		virtual void deleteCharacter() = 0;
		virtual void moveCursorUpDown(bool down) = 0;
		virtual void moveCursorLeftRight(bool left = true) = 0;
		virtual void updateScreen(bool bDelete, char character) = 0; //Only to be used with add and delete char
		int linePosition[2] = { 0, 0 };
		std::vector<std::string> textBuffer = { "" };
		int maxX, maxY;
		int charsPerLine, linesPerPage;
		int pageNum;

		int xPadding, yPadding;

		virtual void printPage(bool up=true) = 0;

	public:

		TextEditorBase(WINDOW* textWin, int yPadding, int xPadding);

		//User mainly interacts with this
		virtual void editorLoop(bool enableEdit) = 0;

		//Setters
		virtual void setPadding(int x, int y) { xPadding = x; yPadding = y; }
		//Getters
		virtual void getPadding(int& x, int& y) { x = xPadding; y = yPadding; }

		virtual std::vector<std::string> getText() = 0;

	};

	class TextEditor : virtual public TextEditorBase
	{
	protected:
		virtual void addCharacter(char character);
		virtual void deleteCharacter();
		virtual void moveCursorUpDown(bool down=true);
		virtual void moveCursorLeftRight(bool left=true);
		virtual void updateScreen(bool bDelete, char character);
		
		virtual bool isOffScreen();

		virtual void printPage(bool up=true);

	public:

		TextEditor(WINDOW* textWin, int yPadding, int xPadding);

		virtual void editorLoop(bool enableEdit=true);

		virtual std::vector<std::string> getText() { return textBuffer; };
	};

	class TEViewer : virtual public TextEditor
	{
	private:
		std::string inputFile;
		
		void parseINFile();

	public:
		TEViewer(WINDOW* textWin, int yPadding, int xPadding, std::string inputFile);

		virtual void commitChanges();
	};
};

#endif //PDCURSES_IO_H
