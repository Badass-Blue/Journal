/*
 * Exceptions.cpp
 *
 *  Created on: 26 Jan. 2018
 *      Author: Zac
 */

#include "Exceptions.h"

void HandleExcep::createLog(std::string dir, std::vector<std::string> line)
	{ B_IO::logFile(dir, line, "", B_IO::IosModes::app ); }
