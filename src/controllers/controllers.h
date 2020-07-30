#ifndef _CONTROLLERS_H
#define _CONTROLLERS_H

#include "../lib/NLTemplate.h"
#include "../core/WebProgram.h"

#include "server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

using namespace NL::Template;
using namespace std;
    
class MissingFieldException : public exception
{
  virtual const char* what() const throw() {
    return "While parse the POST or PUT string, a field was missing.";
  }
};

class EmptyMandatoryFieldException : public exception
{
  public:
    EmptyMandatoryFieldException(string n) : name(n) {}

    virtual const char* what() const throw() {
      return "While parse the POST or PUT string, a mandatory field was empty.";
    }

    string name;
};

class PostRequest {
  public:
    PostRequest(shared_ptr<HttpServer::Request> request) {
      vals = SimpleWeb::QueryString::parse(request->content.string());
    }

    string getVal(string name) {
      auto it = vals.find(name);
      if (it == vals.end()) {
        throw MissingFieldException();
      }
      return it->second;
    }

    string getMandatoryVal(string name) {
      string val = getVal(name);
      if (val.empty()) {
        throw EmptyMandatoryFieldException(name);
      }
      return val;
    }

    string redirect;
     
  protected:  
    SimpleWeb::CaseInsensitiveMultimap vals;
};

#endif
