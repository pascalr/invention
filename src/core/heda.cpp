#include "heda.h"

#include "client_http.hpp"
#include "../lib/opencv.h"
#include <opencv2/highgui.hpp>
#include "log.h"
#include "parser.h"

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

void Heda::stop() {

  auto h5 = Header5("STOP");

  //m_stack.clear();
  //m_pending_commands.clear();
  is_gripping = false;
  m_writer << "s";
  askPosition();
}

void Heda::captureFrame(cv::Mat& frame) {

  HttpClient client("192.168.0.19:8889");
  try {
    auto r1 = client.request("GET", "/slow_capture.jpg");

    //vector<uchar> v(131072); I can't do push_back with this...
    vector<uchar> v;
    while (true) {
      char ch = r1->content.rdbuf()->sgetc();
      if (r1->content.rdbuf()->snextc() == std::streambuf::traits_type::eof()) {break;}
      v.push_back(ch);
    }

    //vector<uchar> decodeBuf(r1->content.rdbuf()->begin(), r1->content.rdbuf()->end());
    //r1->content.rdbuf()->sgetn(decodeBuf, r1->content.size());
    frame = cv::imdecode(cv::Mat(v), cv::IMREAD_COLOR);
  } catch(const SimpleWeb::system_error &e) { // e.what()
    ensure(false, "Unable to capture frame.");
  }
  if (frame.empty()) {
    ensure(false, "Was able to capture frame, but it was empty.");
  }
}

void Heda::askPosition() {
  
  m_writer << "@";

  bool receivedMessagePosition = false;

  while (true) {
    if (m_reader.inputAvailable()) {
      string str = getInputLine(m_reader);

      if (str == MESSAGE_POSITION) {
        receivedMessagePosition = true; continue;

      } else if (receivedMessagePosition) {
        cout << "Parsing position = " << str << endl;
        Parser parser;
        ParseResult result;
        parser.parse(result, "foo " + str); // FIXME: useless command at beginning
        double x = result.popScalaire();
        double y = result.popScalaire();
        double t = result.popScalaire();
        m_position = PolarCoord(x, y, t);
        cout << "Done parsing position. Result: " << m_position << endl;
        return;
      }
    }
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}

void Heda::ensureActive() {
  while(server_reader.inputAvailable()) {
    string cmd = getInputLine(server_reader);
    if (cmd == "stop") {
      stop();
      throw StoppedException();
    }
  }
}

void Heda::shelfByHeight(Shelf& shelf, double userHeight) {

  ensure(!shelves.empty(), "There are no shelves available!");

  order(shelves, byHeight);
  auto previousIt = shelves.begin();
  for (auto it = shelves.begin(); it != shelves.end(); ++it) {

    if (userHeight < it->height) {shelf = *previousIt; return;}
    previousIt = it;
  }
 
  shelf = shelves.back();
  return;
}

UserCoord Heda::toUserCoord(const PolarCoord p, double reference) {

  double offsetX = config.user_coord_offset_x;
  double offsetY = config.user_coord_offset_y;
  double offsetZ = config.user_coord_offset_z;
  // The x axis and the z axis are reverse (hence * -1)
  return UserCoord(p.h - (cosd(p.t) * reference) + offsetX,
              offsetY-p.v,
              ((sind(p.t) * reference)*-1)+offsetZ);
}

PolarCoord Heda::toPolarCoord(const UserCoord c, double reference) {

  double robotZ = -c.z + config.user_coord_offset_z;

  stringstream errorMsg; errorMsg << "toPolarCoord, wrong params, impossible to the given UserCoord, reference is too small. robotZ = ";
  errorMsg << robotZ << ", reference = " << reference;
  ensure(robotZ <= reference, errorMsg.str().c_str());
  
  double t = (asin(robotZ / reference) * 180.0 / PI);
  if (c.x > config.middleX()) { // FIXME: Is X_MIDDLE THE GOOD CONSTANT? Probably not. Use something from heda config.
    t = 180.0 - t;
  }
  double h = c.x - config.user_coord_offset_x + (cosd(t) * reference);

  return PolarCoord(h, unitV(c.y), t);
}

Axis* Heda::axisByName(char name) {
  Axis* axis = 0;
  if (name == 'h' || name == 'H') {
    axis = &axisH;
  } else if (name == 'v' || name == 'V') {
    axis = &axisV;
  } else if (name == 't' || name == 'T') {
    axis = &axisT;
  }
  return axis;
}

void Heda::loadConfig() {
  vector<HedaConfig> configs = db.all<HedaConfig>();
  ensure(!configs.empty(), "Error could not find a valid config.");
  config = *configs.begin();

  shelves = db.all<Shelf>();
  storage_shelves = db.all<Shelf>("WHERE id <> " + to_string(config.working_shelf_id));
  working_shelf = db.find<Shelf>(config.working_shelf_id);
  ensure(working_shelf.exists(), "Error could not find the working shelf.");
}
