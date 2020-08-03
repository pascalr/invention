#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

class WrongTokenTypeException : public exception {};
class ExpectedScalaireException : public exception {};

enum TokenType {
// A unit can be a volume, a mass or a weight.
// cups, t., ...
  UNIT,
// A quantity is the combination of both a scalar and a unit.
  QUANTITY,
  SCALAIRE,
  UNKOWN
};

class TokenValue {
  public:
    virtual ~TokenValue() {}
    virtual TokenType getType() = 0;
};

class Unit : public TokenValue {
  public:
    TokenType getType() {return UNIT;}
};

class Quantity : public TokenValue {
  public:
    TokenType getType() {return QUANTITY;}
};

class Scalaire : public TokenValue {
  public:
    Scalaire(double val) : value(val) {}
    double value;
    TokenType getType() {return SCALAIRE;}
};

// A token must be dynamically allocated. This class encapsulates that.
class Token {

  public:

    Token() {
    }

    ~Token() {
      if (val) {
        delete val;
        /*switch(val->getType()) {
          case UNIT:
            delete (Unit*)val;
            break;
          case SCALAIRE:
            delete (Scalaire*)val;
            break;
          case QUANTITY:
            delete (Quantity*)val;
            break;
        }*/
      }
    }

    double toScalaire() {
    
      if (getType() != SCALAIRE) {
        throw ExpectedScalaireException();
      }
    
      return ((Scalaire*) val)->value;
    }

    void setVal(TokenValue* v) {
      val = v;
    }

    TokenType getType() const {
      if (!val) {return UNKOWN;}
      return val->getType();
    }

    virtual explicit operator bool() const {
      return getType() != UNKOWN;
    }

    TokenValue* val;
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


TokenValue* parseNumber(const string &word) {
  
  char* pEnd;
  double val = strtod (word.c_str(), &pEnd);
  if (strlen(pEnd) == 0) {
    return new Scalaire(val);
  }
  return NULL;
}

Token tokenize(const string &word) {

  Token token;
  if ((token.val = parseNumber(word))) {return token;}
  //if (token.val = parseUnit(word)) {return token;}

  return token;
}

void tokenize(vector<Token> &tokens, const vector<string> &words) {
  for (auto it = words.begin(); it != words.end(); it++) {
    tokens.push_back(tokenize(*it));
  }
}

void parse(string cmd) {
  
  transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower); 
  /*int state;
  VALUE result;
  result = rb_eval_string_protect(cmd, &state);

  if (TYPE(result) == T_STRING) {
    char* str = StringValueCStr(result);
    cout << "Got a string! String = " << str << "\n";
  } else {
    cerr << "Error expected a string.\n";
  }

  if (state) {
    cerr << "There was an exception in ruby: state = " << state << "\n";
  	// handle exception
  }*/
}

#endif
