#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifndef CONSOLECMDS_H
#define CONSOLECMDS_H

#include <memory>
#include <iostream>
#include <string>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <curses.h>
#include <vector>
#include <map>
#include <array>

#include "Boost_IO.h"
#include "PDCurses_IO.h"
#include "Exceptions.h"

using namespace std;
namespace fs = boost::filesystem;

//For colour changer
#define DEFCOLOURS 0
#define ERRCOLOUR  1
#define MSSGCOLOUR 2

class ConsoleController {

private:

	/*Create variables for use in MmenuOpt()*/
	//Short for Words Input
	string WInput;

	//Short for file extension
	const string f_ext = ".page";

	//Shared data between Option 2 and 3
	map<int, fs::path> files;

	//Only for Option 2
	//WINDOW* Opt2Win;
	std::vector<std::string> choices;
	//Determines where menu will be on the stdscr
	int maxY, maxX;
	int starty, startx;
	int height;
	int width;

public:

	/*Create a window for curses*/
	void cInit();

	ConsoleController();

	void colourStart();

	/*Current path*/
	const string rootDir = fs::current_path().string();

	/*Message printer*/
	void mssgprint(vector<const char *> &mssg, bool clearS);
	void mssgprint(const char* mssg, bool clearS);

	vector<const char *> bootm = { "...\n", "Copyright 2017 ...\n", "Created by Zac Henderson\n\n",
		"-------------------------------", "\n\nWelcome to your electronic Journal\n" };

	vector<const char *> MMenum = { "Please choose an option\n\n" };

	const char* Opt0m = "Press ESC key to finish\n";
	const char* Opt0_1m = "\nAn entry already exists for today, do you want to edit it?\nNote, a timestamp will be added (Y or N)\n";

	const char* Opt2m = "Please choose one of the offered entries\n";

	const char* Opt3m = "Please type the directory you want to search\n";

	const char* Opt4m = "Are you sure you want to quit (Y y or N n)?\n";

	void MmenuOpt(bool clear_s=true);

	/*Menu options code*/
	void Opt0(string rootDirect, bool clear_s=true);
	void Opt1(string rootDirect, bool clear_s=true);
	void Opt2(string rootDirect, bool clear_s=true);
	void Opt3(string rootDirect, bool clear_s=true);
	bool Opt4();
};

namespace Interface
{
	extern ConsoleController* CC;
	extern ErrObj* ErrStore;
	extern HandleExcep* ExcepCode;

	extern bool checkLog();
	extern void createDir(std::string dir);
	extern void fileState(std::fstream &file, bool &result, std::string fileName);
	extern void pdColourSupport(bool& result);
	extern bool listEntries(std::map<int, fs::path>& tempMap, const fs::path &path, const std::string fileExtention,
		B_IO::SearchFilter searchFilter=B_IO::SearchFilter::file);
}

#endif //CONSOLECMDS_H
