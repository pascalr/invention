#include "test.h"

#include "../core/parser.h"

using namespace std;

void setup() {
}

void testParseQuantity() {
  title("Testing parse quantity");
}

void testSplitWords() {
  title("Testing split words");

  vector<string> words;
  splitWords(words, "This is test\n");
  assertEqual("Number of words", (unsigned)3, words.size());
  assertEqual("words[0]", "This", words[0]);
  assertEqual("words[1]", "is", words[1]);
  assertEqual("words[2]", "test", words[2]);
}

void testParse() {
  title("Testing parse");

  vector<string> words;
  words.push_back("1234");
  words.push_back("test");

  Parser parser;
  ParseResult result;
  parser.parse(result, "test 1234 foo");

  assertEqual("Command name", "test", result.getCommand());
  assertEqual("1234", 1234, result.popScalaire());
  //assertEqual("test", UNKOWN, tokens[1].getType());
 
  
  ParseResult result1;
  parser.parse(result1, "Move X 1234");
  assertEqual("Command name", "move", result1.getCommand());
  assertEqual("x", 'x', result1.popAxis());
  assertEqual("1234", 1234, result1.popScalaire());

}

/*void assertNumber(double val, const char* str) {

  Token tok = tokenize(str);
  assertEqual(str, SCALAIRE, tok.getType());
  if (tok.getType() == SCALAIRE) {
    assertEqual(str, val, tok.toScalaire());
  }
}

void testParseNumber() {
  title("Testing parse number");
  
  assertNumber(1234, "1234");
  assertNumber(12.34, "12.34");
  assertException("ExpectedScalaireException", []() {tokenize("12.34.").toScalaire();});
  assertException("ExpectedScalaireException", []() {tokenize("1a2").toScalaire();});
}*/

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  testSplitWords();
  //testParseNumber();
  testParse();
  testParseQuantity();

}
