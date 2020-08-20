#ifndef LIB_H 
#define LIB_H

#include <algorithm> // ?
#include <cctype> // ?
#include <locale> // ?
#include <fstream> // ?

#include <string>

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
std::string replaceAll(std::string str, const std::string& from, const std::string& to);

// insensitive equal
bool iequals(const std::string& a, const std::string& b);

#endif // COMMON_H
