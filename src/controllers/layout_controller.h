#ifndef _LAYOUT_CONTROLLER_H
#define _LAYOUT_CONTROLLER_H

#include "../lib/NLTemplate.h"

using namespace NL::Template;

void addLayoutToContent(stringstream& withLayout, string layoutName, stringstream& content) {

  LoaderFile loader;
  Template t( loader );
  t.load(layoutName);

  const char menuStr[] = ""
    "<div id='menu'>"
    "  <a class='item' href='/inventaire.html'>Inventaire</a>"
    "  <a class='item' href='/recettes/index'>Recettes</a>"
    "  <a class='item' href='/ingredients/index'>Ingredients</a>"
    "  <a class='item' href='/axes/index.html'>Manuel</a>"
    "  <a class='item' href='/visuel.html'>Visuel</a>"
    "  <a class='item' href='/3d/index.html'>3d</a>"
    "</div>";

  t.set("menu", menuStr);
  t.set("root", content.str());
  t.render(withLayout);
}



#endif
