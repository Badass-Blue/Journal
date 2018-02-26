/*
 * Exceptions.h
 *
 *  Created on: 10 Jan. 2018
 *      Author: Zac
 */
#include <exception>
#include <vector>
#include <string>
#include "Boost_IO.h"

#ifndef SOURCES_EXCEPTIONS_H_
#define SOURCES_EXCEPTIONS_H_

struct ErrObj
{
	std::vector<std::string> string;
	int code;
};

class HandleExcepBase
{
protected:
	ErrObj* ErrStore;
public:
	HandleExcepBase(ErrObj Err)
		{ ErrStore = &Err; }

	virtual std::vector<std::string> getErrString() = 0;
	virtual int getErrCode() = 0;

	virtual void createLog(std::string dir, std::vector<std::string> line) = 0;
	virtual std::vector<std::string>& getLogLine() = 0;
	virtual void setlogDir(std::string logDir) = 0;
	virtual void setIosMode(B_IO::IosModes ios) = 0;

	template <typename Func>
	bool exceptionWall(Func&& functor, std::string failType);

	virtual ~HandleExcepBase()
		{ delete ErrStore; }
};


class HandleExcep : virtual public HandleExcepBase
{
private:
	std::string logDir;
	std::vector<std::string> line;
	B_IO::IosModes ios;
public:

	HandleExcep(ErrObj Err, std::string logDir) : HandleExcepBase(Err)
	{
		this->logDir = logDir;
		line.push_back( ( "Started application " + B_IO::getDate() ) );
		createLog(logDir, line);
		line.clear();
	}

	virtual std::vector<std::string> getErrString() { return ErrStore->string; }
	virtual int getErrCode() { return ErrStore->code; }

	virtual void createLog(std::string dir, std::vector<std::string> line);
	virtual std::vector<std::string>& getLogLine() { return line; }
	virtual void setlogDir(std::string logDir) { this->logDir = logDir; }
	virtual void setIosMode(B_IO::IosModes ios) { this->ios = ios; }

	#define INFO "Information: "
	#define WARN "Warning: "
	#define ERRO "Error: "

	bool logFail = false;

	template <typename Func>
	void exceptionWall(Func&& functor, std::string failType=ERRO)
	{
		line.clear();
		try
		{
			functor();
		} catch( std::exception &err ) {

			line.push_back( err.what() );
			if( B_IO::logFile(logDir, line, failType) )
				{ logFail = true; }

		} catch( const char* err ) {

			line.push_back(err);
			if( B_IO::logFile(logDir, line, failType) )
				{ logFail = true; }
		} catch( ... ) {
			line.push_back("Unknown error occurred");
			if( B_IO::logFile(logDir, line, failType) )
				{ logFail = true; }
		}
	}
};

#endif /* SOURCES_EXCEPTIONS_H_ */
