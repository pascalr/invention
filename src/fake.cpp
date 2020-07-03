#include <iostream>
#include <cstring>
#include <math.h>
#include <string>

#include "programme/axis.h"
#include "programme/setup.h"
#include "programme/common.h"
#include "io_common.h"

#include "programme/test/matplotlibcpp.h" // FIXME
      
#include <chrono>
#include <thread>
#include <future>

#include "programme/test/console_writer.h"

#include <sys/select.h>

#include "utils.h"

std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool inputAvailable() {
  // https://stackoverflow.com/questions/6171132/non-blocking-console-input-c
  // we want to receive data from stdin so add these file
  // descriptors to the file descriptor set. These also have to be reset
  // within the loop since select modifies the sets.
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(STDIN_FILENO, &read_fds);
  int sfd = 1; // I think, because only 1 file descriptor

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  int result = select(sfd + 1, &read_fds, NULL, NULL, &tv);
  if (result == -1 && errno != EINTR) {
    cerr << "Error in select: " << strerror(errno) << "\n";
  } else if (result == -1 && errno == EINTR) {
    exit(0); // we've received an interrupt - handle this
  } else {
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      return true;
    }
  }
  return false;
}

using namespace std;
namespace plt = matplotlibcpp;

vector<double> toVect(double x) {
  vector<double> xs(1);
  xs[0] = x;
  return xs;
}

void drawToolPosition(double x, double z) {
  plt::plot(toVect(x),toVect(z),"ro");
}

void drawArmCenterAxis(HorizontalAxis* axisX, Axis* axisZ) {
  vector<double> x(2);
  vector<double> z(2);

  x[0] = axisX->getPosition();
  x[1] = axisX->getPosition() - axisX->getDeltaPosition();
  z[0] = axisZ->getPosition();
  z[1] = 0.0;
  plt::plot(x,z,"b-");
}

// TODO: Do my own thing I think, because yeah not sure what lib to use anyways...
class Point {
  public:
    Point(double valX, double valY) : x(valX), y(valY) {}
    Point operator+(Point& v) {
      return Point(x + v.x, y+v.y);
    }

    double x;
    double y;
};

void drawArmBoundingBox(HorizontalAxis* axisX, ZAxis* axisZ) {
  double angle = axisZ->getPositionAngle();

  double a[4][2] = {{0,0},
                   {ARM_WIDTH, 0},
                   {ARM_WIDTH, ARM_LENGTH},
                   {0, ARM_LENGTH}};


}

void drawPossibleXPosition(double maxX) {
  vector<double> x(2);
  vector<double> z(2);
  x[0] = 0.0;
  x[1] = maxX;
  z[0] = 0.0;
  z[1] = 0.0;
  plt::plot(x,z,"k-");
}

void draw(Axis** axes) {
  HorizontalAxis* axisX = (HorizontalAxis*)axisByLetter(axes, 'X');
  ZAxis* axisZ = (ZAxis*)axisByLetter(axes, 'Z');
  
  plt::clf();
  
  drawPossibleXPosition(axisX->getMaxPosition());
  drawArmCenterAxis(axisX, axisZ);
  drawToolPosition(axisX->getPosition(),axisZ->getPosition());

  plt::xlim(-OFFSET_X, ARMOIRE_WIDTH-OFFSET_X);
  plt::ylim(-OFFSET_Z, ARMOIRE_DEPTH-OFFSET_Z);

  plt::title("Position du bras");
  plt::pause(0.01);
}

ConsoleWriter writer = ConsoleWriter();
VerticalAxis axisY = VerticalAxis(writer, 'Y');
HorizontalAxis axisX = HorizontalAxis(writer, 'X');
Axis axisA = Axis(writer, 'A');
Axis axisB = Axis(writer, 'B');
ZAxis axisZ = ZAxis(writer, 'Z', &axisX);
Axis axisT = Axis(writer, 'T');
  
Axis* axes[] = {&axisX, &axisY, &axisT, &axisA, &axisB, &axisZ, NULL};

unsigned long currentTime = 0;
bool isWorking = false;

void setup() {
  setupAxes(&writer, axes);
}

void loop() {
  if (isWorking) {

    if (inputAvailable()) {
      // TODO: Check for stop, or position queries
      // cout << MESSAGE_INVALID << endl;
      string str;
      cin >> str;
      if (str.length() >= 10) {
        writer << MESSAGE_INVALID << '\n';
        return;
      }
      char c_str[12];
      strcpy(c_str, str.c_str());
      rtrim(c_str);
      writer << MESSAGE_RECEIVED << '\n';
      if (strlen(c_str) != 1) {
        writer << MESSAGE_INVALID << '\n';
        return;
      }
      char cmd = c_str[0];
      if (cmd == 's' || cmd == 'S') {
        for (int i = 0; axes[i] != NULL; i++) {
          axes[i]->stop();
        }
        writer << MESSAGE_DONE << '\n';
        isWorking = false;
      } else if (cmd == '?') {
        for (int i = 0; axes[i] != NULL; i++) {
          axes[i]->serialize();
        }
        writer << MESSAGE_DONE << '\n';
      } else if (cmd == '@') { // asking for position
        writer << MESSAGE_INVALID << '\n';
      } else {
        writer << MESSAGE_INVALID << '\n';
      }
      return;
    }

    bool stillWorking = false;
    for (int i = 0; axes[i] != NULL; i++) {
      stillWorking = stillWorking || axes[i]->handleAxis(currentTime);
    }

    if (!stillWorking) {
      isWorking = false;
      writer << MESSAGE_DONE << '\n';
    }
    
  } else if (!inputAvailable()) {
    this_thread::sleep_for(chrono::milliseconds(10));
    return;
  } else {
    string cmd;
    cin >> cmd;
    cout << MESSAGE_RECEIVED << '\n';

    isWorking = true;
    
    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->prepare(currentTime);
    }
    
    const char* input = cmd.c_str();
    int cursorPos = parseInput(input, &writer, axes, 0);

    for (int i = 0; axes[i] != NULL; i++) {
      axes[i]->afterInput();
    }

  }
}

int main (int argc, char *argv[]) {

  signal(SIGINT, signalHandler);
  setup();

  cerr << ">> ";
  draw(axes);

  while (true) {
    bool wasWorking = isWorking;

    currentTime = (isWorking) ? currentTime + 5 : 0;

    loop();
    
    if (isWorking && currentTime % 200000 == 0) {
      draw(axes);
    }
    if (wasWorking && !isWorking) {
      cerr << ">> ";
      draw(axes);
    }
  }

  return 0;
}
