#ifndef IO_COMMON_H
#define IO_COMMON_H

#include <signal.h>

void signalHandler( int signum ) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  exit(signum);
}

#endif
