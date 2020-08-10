#include "parser.h"

#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

const char* tokenTypeName(TokenType e) {
  switch (e) {
    case UNIT: return "UNIT";
    case QUANTITY: return "QUANTITY";
    case SCALAIRE: return "SCALAIRE";
    case AXIS: return "AXIS";
    case UNKOWN: return "UNKOWN";
    case POSITIVE_INTEGER: return "POSITIVE_INTEGER";
    default: throw "Bad TokenType";
  }
}

void splitWords(vector<string> &words, const string &str) {

  istringstream ss(str);
  do {
    string word;
    ss >> word;
    words.push_back(word);
  } while (ss);

  if (!words.empty()) {
    words.pop_back();
  }
}

void ParseResult::expectArgument() {
  if (m_tokens.empty()) {
    throw MissingArgumentException();
  }
}

void ParseResult::expectArgument(TokenType type) {

  expectArgument();

  TokenType actual = (*m_tokens.begin())->getType();
  if (actual != type) {
    throw WrongTypeArgumentException(type, actual);
  }
}

bool ParseResult::checkArgument(TokenType type) {

  expectArgument();

  shared_ptr<Token> token = *m_tokens.begin();
  return token->getType() == type;
}

char ParseResult::popAxis() {

  expectArgument(AXIS);
  
  double val = (dynamic_pointer_cast<AxisToken> (*m_tokens.begin()))->name;
  m_tokens.erase(m_tokens.begin());

  return val;
}

unsigned long ParseResult::popPositiveInteger() {

  expectArgument(POSITIVE_INTEGER);
  
  unsigned long val = (dynamic_pointer_cast<PositiveInteger> (*m_tokens.begin()))->value;
  m_tokens.erase(m_tokens.begin());

  return val;
}

double ParseResult::popScalaire() {

  if (checkArgument(POSITIVE_INTEGER)) {return popPositiveInteger();}
 
  expectArgument(SCALAIRE);

  double val = (dynamic_pointer_cast<Scalaire> (*m_tokens.begin()))->value;
  m_tokens.erase(m_tokens.begin());

  return val;
}

void ParseResult::addToken(shared_ptr<Token> tok) {
  m_tokens.push_back(tok);
}

void ParseResult::setCommand(const string &str) {
  m_command = str;
}

string ParseResult::getCommand() const {
  return m_command;
}

bool Parser::parseScalaire(ParseResult &result, const string &word) {
  
  char* pEnd;
  double val = strtod (word.c_str(), &pEnd);
  if (strlen(pEnd) != 0) { return false;}

  shared_ptr<Token> tok = make_shared<Scalaire>(val);
  result.addToken(tok);
  return true;
}

bool Parser::parsePositiveInteger(ParseResult &result, const string &word) {

  for (auto it = word.begin(); it != word.end(); it++) {
    if (*it < '0' || *it > '9') {return false;}
  }

  shared_ptr<Token> tok = make_shared<PositiveInteger>(atoi(word.c_str()));
  result.addToken(tok);
  return true;
}

bool Parser::parseAxis(ParseResult &result, const string &word) {

  if (word == "x" || word == "y" || word == "z" || word == "t") {

    shared_ptr<Token> tok = make_shared<AxisToken>(word[0]);
    result.addToken(tok);
    return true;
  }
  
  return false;
}

bool Parser::parseUnkown(ParseResult &result, const string &word) {

  shared_ptr<Token> tok = make_shared<Unkown>(word);
  result.addToken(tok);
  
  return true;
}

void Parser::tokenize(ParseResult &result, const string &word) {

  parsePositiveInteger(result, word) || parseScalaire(result, word) || parseAxis(result, word) || parseUnkown(result, word);
}


void Parser::parse(ParseResult &result, const string cmd1) {
 
  string cmd = cmd1; 
  transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower); 

  vector<string> words;
  splitWords(words, cmd);

  if (words.size() < 1) {
    throw EmptyCommandException();
  }

  result.setCommand(*words.begin());
  words.erase(words.begin());

  for (auto it = words.begin(); it != words.end(); it++) {
    tokenize(result, *it);
  }
}
