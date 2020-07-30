#ifndef _RECETTES_CONTROLLER_H
#define _RECETTES_CONTROLLER_H

#include "../lib/NLTemplate.h"
#include "controllers.h"

using namespace NL::Template;

namespace Recettes {

  void index(WebProgram& wp, Template& t) {
  
    t.load("frontend/recettes/index.html");

    vector<Recette> recettes;
    wp.db.load(recettes);

    int size = recettes.size();
    t.block("recettes").repeat(size);

    for ( int i=0; i < size; i++ ) {

      Recette recette = recettes[i];
      t.block("recettes")[i].set("name", recette.name);
    }
  }

  void remove(WebProgram& wp, Template& t) {
    
    string name = wp.request->path_match[1].str();
    Recette recette(name);
    wp.db.remove(recette, name);
  }

  void show(WebProgram& wp, Template& t) {

    string name = wp.request->path_match[1].str();
    Recette recette;
    wp.db.get(recette, name);
    
    t.load("frontend/recettes/show.html");
    t.set("name", recette.name);

    string instructions = recette.instructions;
    instructions = replaceAll(instructions, "\n", "<br />");
    t.set("instructions", instructions);

  }
  
  void edit(WebProgram& wp, Template& t) {

    string name = wp.request->path_match[1].str();
    Recette recette;
    wp.db.get(recette, name);
    cout << "Instructions: " << recette.instructions << endl;

    t.load("frontend/recettes/edit.html");
    t.set("name", recette.name);
    t.set("instructions", recette.instructions);
  }

  void create(WebProgram& wp, Template& t) {
    t.load("frontend/recettes/new.html");
  }

  void do_create(WebProgram& wp, PostRequest& request) {

    string name = request.getMandatoryVal("name");

    std::cout << "Do_create name: " << name << std::endl;

    Recette ingredient(name);
    wp.db.save(ingredient);
    
    request.redirect = "/recettes/edit/"+name; // TODO Encode name
  }

  void do_update(WebProgram& wp, PostRequest& request) {

    std::cout << "Do_update " << std::endl;

    string oldName = wp.request->path_match[1].str();
    Recette oldRecette;
    wp.db.get(oldRecette, oldName);

    Recette recette;
    recette.name = request.getMandatoryVal("name");
    recette.instructions = request.getVal("instructions");
    recette.rowid = oldRecette.rowid;
    wp.db.save(recette);

    std::cout << "Do_update name: " << recette.name << std::endl;
    
    request.redirect = "/recettes/show/"+recette.name; // TODO Encode name
  }

}

#endif
