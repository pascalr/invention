#ifndef _COMM_H
#define _COMM_H

#include "../core/reader/reader.h"

bool isArduinoReady(Reader &reader);

void waitUntilDone(Reader& reader);

std::string readAllUntilDone(Reader& reader);

#endif
