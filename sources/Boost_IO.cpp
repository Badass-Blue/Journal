#include "Boost_IO.h"

namespace fs=boost::filesystem;

/*Function for retrieving local time*/
std::string B_IO::getDate(const char* format)
{
	time_t rawtime;
	time(&rawtime);
	struct tm timeinfo;
	char the_date[80];
	localtime_s(&timeinfo, &rawtime);

	//Check to see if time_t didn't fail
	if(rawtime != -1)
		strftime(the_date, 80, format, &timeinfo);

	return the_date;
}

const char* B_IO::AccessException::what() const throw()
{
	return "The provided directory, or file, either doesn't exist,\n" \
			"or this program doesn't have permission to access it\n";
}

/*File System Explorer*/
bool B_IO::ListEntries(std::map<int, fs::path>& tempMap, const fs::path& root, const std::string ext, SearchFilter filter)
{
	std::map<int, fs::path> searchedIndex;
	if (!fs::exists(root))
	{
		throw B_IO::AccessException();
		return false;
	}

	//OPTIMISE THIS FOR LOOP AND IF(EXT != "")
	int i = 0;
	for (fs::directory_iterator it(root); it != fs::end(it); ++it)
	{
		switch(filter)
		{
			case directory:
				if (fs::is_directory(it->path()))
					{ searchedIndex[i] = it->path(); ++i; }
				break;
			case file:
				if (fs::is_regular_file(it->path()))
				{
					if (ext != "")
					{
						if ( it->path().extension() == ext )
							{ searchedIndex[i] = it->path().stem(); ++i; }
					}
					else
						{ searchedIndex[i] = it->path().stem(); ++i; }
				}
				break;
			case none:
				searchedIndex[i] = it->path(); ++i;
				break;
		}
	}
	tempMap = searchedIndex;
	return true;
}

const char* B_IO::CreateDirException::what() const throw()
{
	return "Cannot create directory\n";
}

void B_IO::CreateDir(std::string dir)
{
	if(!fs::create_directories(dir))
		{ throw B_IO::CreateDirException(); }
}

bool B_IO::logFile(std::string dir, std::vector<std::string> line, std::string preAppend, IosModes ios)
{
	//ios open_modes:
	//	app    => set output position to end of stream before every operation
	//	ate    => set output position to end of stream on opening
	//	binary => consider stream as binary instead of text
	//	in     => enable input on stream
	//	out    => enable output on stream
	//	trunc  => destroy everything upon operation, and assume length of zero upon opening

	std::fstream LogFile;
	switch(ios)
	{
	case IosModes::app:
	{
		LogFile.open(dir, std::ios::app);
		break;
	}
	case IosModes::ate:
	{
		LogFile.open(dir, std::ios::app | std::ios::ate);
		break;
	}
	case IosModes::binary:
	{
		LogFile.open(dir, std::ios::binary);
		break;
	}
	case IosModes::in:
	{
		LogFile.open(dir, std::ios::in);
		break;
	}
	case IosModes::out:
	{
		LogFile.open(dir, std::ios::out);
		break;
	}
	case IosModes::trunc:
	{
		LogFile.open(dir, std::ios::trunc);
		break; }
	}

	if( LogFile.is_open() )
	{
		for(auto it : line)
		{
			LogFile << preAppend << it << "\n";
		}
	}
	else { LogFile.close(); return true; }
	LogFile.close();

	return false;
}
