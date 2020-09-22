#ifndef _TEST_H
#define _TEST_H

#include "../lib/lib.h"
#include "../utils/io_common.h"
#include <iostream>
#include <functional>

using namespace std;
  
void assertException(const char* exceptionClassName, std::function<void()> toCall) {
  try {
    toCall();
    cout << "\033[31mFAILED\033[0m";
  } catch (const exception& e) {
    // TODO: Check the exception class name
    cout << "\033[32mPASSED\033[0m";
  }
  //cout << " (" << info << ")";
  cout << " - Expected an exception to occur of type " << exceptionClassName << endl;
}

template <class P1, class P2>
void assertNearby(const char* info, P1 t1, P2 t2) {
  cout << ((t1 - t2) < 0.5 && (t2 - t1) < 0.5 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertEqual(const char* info, P1 t1, P2 t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

// assertTest is the old name for assertEqual

template <class P1, class P2>
void assertTest(const char* info, P1 t1, P2 t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertTest(P1 t1, P2 t2) {
  cout << (t1 == t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

void title(const char* str) {
  cout << "\033[36;1m" << str << "\033[0m" << endl;
}

template <class P1, class P2>
void assertNotEqual(const char* info, P1 t1, P2 t2) {
  cout << (t1 != t2 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertSmaller(const char* info, P1 t1, P2 t2) {
  cout << (t2 < t1 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

template <class P1, class P2>
void assertGreater(const char* info, P1 t1, P2 t2) {
  cout << (t2 > t1 ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");
  cout << " (" << info << ")";
  cout << " - Expected: " << t1 << ", " << "Got: " << t2 << endl;
}

#endif
