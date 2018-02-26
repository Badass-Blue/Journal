#ifndef BOOST_IO_H
#define BOOST_IO_H

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <exception>

namespace fs=boost::filesystem;

namespace B_IO
{
	/*Function for retrieving local time*/
	std::string getDate(const char* format="%d %m %Y");

	class AccessException : virtual public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	/*File System Explorer*/
	enum SearchFilter
	{
		directory,
		file,
		none
	};
	bool ListEntries(std::map<int, fs::path>& tempMap, const fs::path& root, const std::string ext="",
		SearchFilter filter=file);

	class CreateDirException : virtual public std::exception
	{
	public:
		virtual const char* what() const throw();
	};

	void CreateDir(std::string dir);

	enum IosModes
	{
		app,
		ate,
		binary,
		in,
		out,
		trunc
	};
	bool logFile(std::string dir, std::vector<std::string> line, std::string preAppend="",
			IosModes ios=IosModes::app);
}


#endif //BOOST_IO_H
