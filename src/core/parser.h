#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

class WrongTokenTypeException : public exception {};
class MissingExpectedScalaireException : public exception {};
class WrongTypeExpectedScalaireException : public exception {};
class EmptyCommandException : public exception {};

enum TokenType {
// A unit can be a volume, a mass or a weight.
// cups, t., ...
  UNIT,
// A quantity is the combination of both a scalar and a unit.
  QUANTITY,
  SCALAIRE,
  UNKOWN
};

class Token {
  public:
    virtual ~Token() {}
    virtual TokenType getType() = 0;
};

class Unit : public Token {
  public:
    TokenType getType() {return UNIT;}
};

class Quantity : public Token {
  public:
    TokenType getType() {return QUANTITY;}
};

class Scalaire : public Token {
  public:
    Scalaire(double val) : value(val) {}
    double value;
    TokenType getType() {return SCALAIRE;}
};



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

class ParseResult {
  public:

    double popScalaire() {

      if (m_tokens.empty()) {
        throw MissingExpectedScalaireException();
      }
   
      shared_ptr<Token> token = *m_tokens.begin();
      if (token->getType() != SCALAIRE) {
        throw WrongTypeExpectedScalaireException();
      }
      
      double val = (dynamic_pointer_cast<Scalaire> (token))->value;
      m_tokens.erase(m_tokens.begin());
    
      return val;
    }

    void addToken(shared_ptr<Token> tok) {
      m_tokens.push_back(tok);
    }

    void setCommand(const string &str) {
      m_command = str;
    }

    string getCommand() const {
      return m_command;
    }

    /*vector<Token> getTokens() const {
      return m_tokens;
    }*/

  protected:

    string m_command;
    vector<shared_ptr<Token>> m_tokens;
};

class Parser {
  public:

    bool parseNumber(ParseResult &result, const string &word) {
      
      char* pEnd;
      double val = strtod (word.c_str(), &pEnd);
      if (strlen(pEnd) == 0) {

        shared_ptr<Token> tok = make_shared<Scalaire>(val);
        result.addToken(tok);

        return true;
      }
      return false;
    }

    void tokenize(ParseResult &result, const string &word) {
    
      parseNumber(result, word); // || parseUnit(word) || parse...
    }
    
    
    void parse(ParseResult &result, const string cmd1) {
     
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

};

#endif
