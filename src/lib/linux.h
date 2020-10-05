#ifndef _LINUX_H
#define _LINUX_H

#include <string>

long long currentTimestampMs();

// https://stackoverflow.com/questions/4656824/get-lan-ip-and-print-it
int getLanAddress(std::string& retAddress);

#endif
