#include <iostream>
#include <signal.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>

void signalHandler( int signum ) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  exit(signum);
}

bool linuxInputAvailable() {
  // https://stackoverflow.com/questions/6171132/non-blocking-console-input-c
  // we want to receive data from stdin so add these file
  // descriptors to the file descriptor set. These also have to be reset
  // within the loop since select modifies the sets.
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(STDIN_FILENO, &read_fds);
  int sfd = 1; // I think, because only 1 file descriptor

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  int result = select(sfd + 1, &read_fds, NULL, NULL, &tv);
  if (result == -1 && errno != EINTR) {
    std::cerr << "Error in select: " << strerror(errno) << "\n";
  } else if (result == -1 && errno == EINTR) {
    exit(0); // we've received an interrupt - handle this
  } else {
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      return true;
    }
  }
  return false;
}

std::string nextFilename(std::string dir, std::string base, std::string ext) {
  std::string outfile_name = base;
  for (int i = 1; i < 1000; i++) {
    std::stringstream ss;
    ss << base << "_" << std::setw(3) << std::setfill('0') << i << ext;

    std::stringstream toCheck;
    toCheck << dir << ss.str();

    if (!boost::filesystem::exists(toCheck.str())) {return ss.str();}
  }
  return base + "_moreThan1000" + ext;
}
