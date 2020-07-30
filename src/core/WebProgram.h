#ifndef _WEB_PROGRAM_H
#define _WEB_PROGRAM_H

#include "Recette.h"
#include <vector>
#include "Database.h"

#include "server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

using namespace std;

// Rename this.
// This is passed to every controller function
class WebProgram {
  public:
    WebProgram() : db("data/test.db") {
    }

    //vector<Aliment> aliments;
    //vector<Ingredient> ingredients;
    //vector<Recette> recettes;
    Database db;

    shared_ptr<HttpServer::Request> request;
};

#endif
