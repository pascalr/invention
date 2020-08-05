#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

enum TokenType {
// A unit can be a volume, a mass or a weight.
// cups, t., ...
  UNIT,
// A quantity is the combination of both a scalar and a unit.
  QUANTITY,
  SCALAIRE,
  AXIS,
  POSITIVE_INTEGER,
  UNKOWN
};

enum MyEnum {VAL1, VAL2,VAL3};

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

class MissingArgumentException : public exception {
  public:
};
class WrongTypeArgumentException : public exception {
  public:
    WrongTypeArgumentException(TokenType expected, TokenType actual) {
      stringstream ss; ss << "WrongTypeArgumentException happened. Expected "
                          << tokenTypeName(expected) << ", but got " 
                          << tokenTypeName(actual) << ".\n";
      error = ss.str();
    }

    const char* what() const throw() {
      return error.c_str();
    }

    string error;
};

class WrongTokenTypeException : public exception {};
class EmptyCommandException : public exception {};

// TODO: namespace Token. Token::Unit

class Token {
  public:
    virtual ~Token() {}
    virtual TokenType getType() = 0;
};

class Unit : public Token {
  public:
    TokenType getType() {return UNIT;}
};

class AxisToken : public Token {
  public:
    AxisToken(char name) : name(name) {}
    TokenType getType() {return AXIS;}
    char name;
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

class PositiveInteger : public Token {
  public:
    PositiveInteger(unsigned long val) : value(val) {}
    unsigned long value;
    TokenType getType() {return POSITIVE_INTEGER;}
};

class Unkown : public Token {
  public:
    Unkown(const string &val) : value(val) {}
    string value;
    TokenType getType() {return UNKOWN;}
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

    void expectArgument() {
      if (m_tokens.empty()) {
        throw MissingArgumentException();
      }
    }

    void expectArgument(TokenType type) {

      expectArgument();

      TokenType actual = (*m_tokens.begin())->getType();
      if (actual != type) {
        throw WrongTypeArgumentException(type, actual);
      }
    }

    bool checkArgument(TokenType type) {

      expectArgument();

      shared_ptr<Token> token = *m_tokens.begin();
      return token->getType() == type;
    }

    char popAxis() {

      expectArgument(AXIS);
      
      double val = (dynamic_pointer_cast<AxisToken> (*m_tokens.begin()))->name;
      m_tokens.erase(m_tokens.begin());
    
      return val;
    }

    unsigned long popPositiveInteger() {

      expectArgument(POSITIVE_INTEGER);
      
      unsigned long val = (dynamic_pointer_cast<PositiveInteger> (*m_tokens.begin()))->value;
      m_tokens.erase(m_tokens.begin());
    
      return val;
    }

    double popScalaire() {

      if (checkArgument(POSITIVE_INTEGER)) {return popPositiveInteger();}
     
      expectArgument(SCALAIRE);

      double val = (dynamic_pointer_cast<Scalaire> (*m_tokens.begin()))->value;
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


    bool parseScalaire(ParseResult &result, const string &word) {
      
      char* pEnd;
      double val = strtod (word.c_str(), &pEnd);
      if (strlen(pEnd) != 0) { return false;}

      shared_ptr<Token> tok = make_shared<Scalaire>(val);
      result.addToken(tok);
      return true;
    }

    bool parsePositiveInteger(ParseResult &result, const string &word) {

      for (auto it = word.begin(); it != word.end(); it++) {
        if (*it < '0' || *it > '9') {return false;}
      }

      shared_ptr<Token> tok = make_shared<PositiveInteger>(atoi(word.c_str()));
      result.addToken(tok);
      return true;
    }

    bool parseAxis(ParseResult &result, const string &word) {

      if (word == "x" || word == "y" || word == "z" || word == "t") {

        shared_ptr<Token> tok = make_shared<AxisToken>(word[0]);
        result.addToken(tok);
        return true;
      }
      
      return false;
    }

    bool parseUnkown(ParseResult &result, const string &word) {

      shared_ptr<Token> tok = make_shared<Unkown>(word);
      result.addToken(tok);
      
      return true;
    }

    void tokenize(ParseResult &result, const string &word) {
    
      parsePositiveInteger(result, word) || parseScalaire(result, word) || parseAxis(result, word) || parseUnkown(result, word);
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
