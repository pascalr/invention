#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <string>

class Header1 {
  public:
    Header1(std::string name) : name(name) {
      std::cout << "\033[38;5;215m********** " << name << " **********\033[0m" << std::endl;
    }
    ~Header1() {
      std::cout << "\033[38;5;215m_" << name << "_\033[0m" << std::endl;
    }
    std::string name;
};

class Header2 {
  public:
    Header2(std::string name) : name(name) {
      std::cout << "\033[38;5;215m***** " << name << " *****\033[0m" << std::endl;
    }
    ~Header2() {
      std::cout << "\033[38;5;215m_" << name << "_\033[0m" << std::endl;
    }
    std::string name;
};

class Header3 {
  public:
    Header3(std::string name) : name(name) {
      std::cout << "\033[38;5;215m*** " << name << " ***\033[0m" << std::endl;
    }
    ~Header3() {
      std::cout << "\033[38;5;215m_" << name << "_\033[0m" << std::endl;
    }
    std::string name;
};

class Header4 {
  public:
    Header4(std::string name) : name(name) {
      std::cout << "\033[38;5;215m** " << name << " **\033[0m" << std::endl;
    }
    ~Header4() {
      std::cout << "\033[38;5;215m_" << name << "_\033[0m" << std::endl;
    }
    std::string name;
};

class Header5 {
  public:
    Header5(std::string name) : name(name) {
      std::cout << "\033[38;5;215m* " << name << " *\033[0m" << std::endl;
    }
    ~Header5() {
      std::cout << "\033[38;5;215m_" << name << "_\033[0m" << std::endl;
    }
    std::string name;
};

#endif
