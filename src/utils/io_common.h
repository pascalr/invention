#ifndef IO_COMMON_H
#define IO_COMMON_H

#include <signal.h> // Kept here so SIGINT is defined
#include <string>

void signalHandler( int signum );
bool linuxInputAvailable();
std::string nextFilename(std::string base, std::string ext);

#endif
