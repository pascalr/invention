#ifndef _COMM_H
#define _COMM_H

#include <string>
#include "../core/reader/reader.h"
#include "../core/writer/writer.h"

bool isArduinoReady(Reader &reader);

void waitUntilDone(Reader& reader);

std::string readAllUntilDone(Reader& reader);

std::string getArduinoVersion(Writer& writer, Reader &reader);

bool readProperty(const std::string &name, std::string &result, Reader &reader, int nbAttempts, long sleepMs);

#endif
