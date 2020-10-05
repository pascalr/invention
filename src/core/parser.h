#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <vector>
#include <sstream>
#include <bits/stdc++.h>

// Maybe change the parser. Because all I want to do with is is call popSomething.
// So only the functions popSomething should be implemented, and the args are parse one at a time...

enum TokenType {
// A unit can be a volume, a mass or a weight.
// cups, t., ...
  UNIT,
// A quantity is the combination of both a scalar and a unit.
  QUANTITY,
  SCALAIRE,
  AXIS,
  POSITIVE_INTEGER,
  NOUN, // A noun is a word composed of either a letter, a number and a dash. Orange. Vitamin-B-12. Or an underscore
  UNKOWN
};

const char* tokenTypeName(TokenType e);

class MissingArgumentException : public std::exception {
  public:
};
class WrongTypeArgumentException : public std::exception {
  public:
    WrongTypeArgumentException(TokenType expected, TokenType actual) {
      std::stringstream ss; ss << "WrongTypeArgumentException happened. Expected "
                          << tokenTypeName(expected) << ", but got " 
                          << tokenTypeName(actual) << ".\n";
      error = ss.str();
    }

    const char* what() const throw() {
      return error.c_str();
    }

    std::string error;
};

class WrongTokenTypeException : public std::exception {};
class EmptyCommandException : public std::exception {};

// TODO: namespace Token.

class Token {
  public:
    virtual ~Token() {}
    virtual TokenType getType() = 0;
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

class Noun : public Token {
  public:
    Noun(const std::string &val) : value(val) {}
    std::string value;
    TokenType getType() {return NOUN;}
};

class Unkown : public Token {
  public:
    Unkown(const std::string &val) : value(val) {}
    std::string value;
    TokenType getType() {return UNKOWN;}
};



void splitWords(std::vector<std::string> &words, const std::string &str);

class ParseResult {
  public:

    void expectArgument();

    void expectArgument(TokenType type);

    bool checkArgument(TokenType type);

    char popAxis();

    unsigned long popPositiveInteger();

    double popScalaire();
    
    std::string popNoun();

    void addToken(std::shared_ptr<Token> tok);

    void setCommand(const std::string &str);

    std::string getCommand() const;

    /*std::vector<Token> getTokens() const {
      return m_tokens;
    }*/

  protected:

    std::string m_command;
    std::vector<std::shared_ptr<Token>> m_tokens;
};

class Parser {
  public:


    bool parseNoun(ParseResult &result, const std::string &word);

    bool parseScalaire(ParseResult &result, const std::string &word);
      
    bool parsePositiveInteger(ParseResult &result, const std::string &word);

    bool parseAxis(ParseResult &result, const std::string &word);

    bool parseUnkown(ParseResult &result, const std::string &word);

    void tokenize(ParseResult &result, const std::string &word);
    
    void parse(ParseResult &result, const std::string cmd1);
     
};

#endif
