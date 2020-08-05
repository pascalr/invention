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

  Parser parser;
  /*ParseResult result;
  parser.parse(result, "test 1234 foo");

  assertEqual("Command name", "test", result.getCommand());
  assertEqual("1234", 1234, result.popScalaire());*/
  
  ParseResult result1;
  parser.parse(result1, "move t -8");
  assertEqual("Command name", "move", result1.getCommand());
  assertEqual("t", 't', result1.popAxis());
  assertEqual("-8", -8, result1.popScalaire());

}

void assertParseScalaire(const char* str, double val) {

  Parser parser;
  ParseResult result;
  parser.parseScalaire(result, str);
  assertEqual(str, val, result.popScalaire());
}

void testParseNumber() {
  title("Testing parse number");
  
  assertParseScalaire("1234", 1234.0);
  assertParseScalaire("12.34", 12.34);
  assertParseScalaire("123", 123.0);
  assertParseScalaire("-123", -123.0);
  assertParseScalaire("+123", +123.0);
  assertParseScalaire("123.45", 123.45);
  assertParseScalaire("123.45e2", 12345.0);
  assertParseScalaire("123.45E2", 12345.0);
  assertParseScalaire("123.45e+2", 12345.0);
  assertParseScalaire("0.1e-1", 0.01);
  assertException("ExpectedScalaireException 12.34.", []() {assertParseScalaire("12.34.", 0.0);});
  assertException("ExpectedScalaireException 1a2", []() {assertParseScalaire("1a2", 0.0);});
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

  //testSplitWords();
  //testParseNumber();
  testParse();
  //testParseQuantity();

}
