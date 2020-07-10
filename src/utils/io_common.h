#ifndef IO_COMMON_H
#define IO_COMMON_H

#include <signal.h> // Kept here so SIGINT is defined

void signalHandler( int signum );
bool linuxInputAvailable();

#endif
