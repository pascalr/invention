//#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include <future>

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <thread>
#include <chrono>
#include <exception>

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

bool initVideoDevice(cv::VideoCapture& cap) {
  int deviceID = 0;             // 0 = open default camera
  int apiID = cv::CAP_ANY;      // 0 = autodetect default API
  cap.open(deviceID + apiID);
  // 2048 × 1536
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 2048);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1536);
  cout << "Capturing at resolution "<< cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x" << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << "...\n";
  if (!cap.isOpened()) {
    return false;
  }
  return true;
}

class InitVideoSlaveException : public std::exception {};
class InitSerialPortSlaveException : public std::exception {};

//void tryDetectCodes(vector<HRCode>& positions, int attemptsLeft = 10) {
//
//  cv::Mat frame;
//  captureVideoImage(frame);
//  findHRCodes(frame, positions, 100);
//
//  if (attemptsLeft >= 1 && positions.empty()) {tryDetectCodes(positions, attemptsLeft-1);}
//}
  
int main(int argc, char** argv) {

  HttpServer server;
  server.config.address = "192.168.0.19";
  server.config.port = 8889;
  
  // TODO: Always check if it is opened before taking a picture and send back to the server if there was an error.  
  cv::VideoCapture cap;
  initVideoDevice(cap);
  if (!cap.isOpened()) {
    throw InitVideoSlaveException();
  }

  //SerialPort serialPort;
  //if (serialPort.openPort("/dev/ttyACM0") < 0) {
  //  throw InitSerialPortSlaveException();
  //}
  //
  //SerialReader serialReader(serialPort);
  //SerialWriter serialWriter(serialPort);

  //server.resource["^/detect.jpg$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

  //  vector<HRCode> positions;
  //  tryDetectCodes(positions);

  //  if (positions.empty()) {
  //    cout << "No codes detected..." << endl;
  //    response->write(SimpleWeb::StatusCode::client_error_bad_request, "No codes detected");
  //    return;
  //  }

  //  cout << "Detected one HRCode!!!" << endl;
  //  Mat sideBySide = positions.begin()->img.clone();
  //  string detectedWidths = to_string(sideBySide.cols);
  //  for (auto it = next(positions.begin()); it != positions.end(); ++it) {
  //    cout << "Detected another HRCode!!!" << endl;
  //    hconcat(sideBySide, it->img, sideBySide);
  //    detectedWidths += "," + to_string(it->img.cols);
  //  }

  //  cout << "Here" << endl;

  //  //imshow("test", sideBySide);
  //  //waitKey(0);

  //  vector<uchar> encodeBuf(131072);
  //  imencode(".jpg",sideBySide,encodeBuf);
  //  char* buf = reinterpret_cast<char*>(encodeBuf.data());
  //  streamsize ss = static_cast<streamsize>(encodeBuf.size());
  //  
  //  SimpleWeb::CaseInsensitiveMultimap header;
  //  header.emplace("Content-Length", to_string(encodeBuf.size()));
  //  header.emplace("Content-Type", "image/jpeg");
  //  //header.emplace("Nb-Detectd", to_string(detectedWidths.size()));
  //  //header.emplace("Detected-Widths", detectedWidths);

  //  response->write(SimpleWeb::StatusCode::success_ok, header);
  //  
  //  response->write(buf, ss);
  //  
  //  cout << "THere" << endl;
  //};

  //// Sleeps for 250 ms then captures.
  //server.resource["^/slow_capture.jpg$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
  //  //cout << "GET /cam_capture" << endl;
  //  
  //  vector<uchar> encodeBuf(131072);
  //  captureJpeg(encodeBuf);
  //  char* buf = reinterpret_cast<char*>(encodeBuf.data());
  //  streamsize ss = static_cast<streamsize>(encodeBuf.size());

  //  SimpleWeb::CaseInsensitiveMultimap header;
  //  header.emplace("Content-Length", to_string(encodeBuf.size()));
  //  header.emplace("Content-Type", "image/jpeg");
  //  //response->write(header, encodeBuf.size());
  //  response->write(SimpleWeb::StatusCode::success_ok, header);
  //  response->write(buf, ss);
  //};
 
  // Executing is easy, the issue is reading. 
  // Leave as is for now socat is fine. Later anyway everything will run on the raspberry pi.
  //server.resource["^/exe$"]["POST"] = [&cap](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
  //};

  server.resource["^/capture.jpg$"]["GET"] = [&cap](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    //cout << "GET /cam_capture" << endl;
  
    // Sleep a little to make sure the are little vibrations left
    // in the arm in order to take a still picture. 
    this_thread::sleep_for(chrono::milliseconds(1000)); 
    
    cv::Mat frame;
    cap.read(frame);
    vector<uchar> encodeBuf(131072);
    imencode(".jpg",frame,encodeBuf);
    char* buf = reinterpret_cast<char*>(encodeBuf.data());
    streamsize ss = static_cast<streamsize>(encodeBuf.size());

    SimpleWeb::CaseInsensitiveMultimap header;
    header.emplace("Content-Length", to_string(encodeBuf.size()));
    header.emplace("Content-Type", "image/jpeg");
    //response->write(header, encodeBuf.size());
    response->write(SimpleWeb::StatusCode::success_ok, header);
    response->write(buf, ss);
  };

  server.default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path);
  };

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
  };

  // Start server and receive assigned port when server is listening for requests
  promise<unsigned short> server_port;
  thread server_thread([&server, &server_port]() {
    // Start server
    server.start([&server_port](unsigned short port) {
      server_port.set_value(port);
    });
  });
  cout << "Server listening on " << server.config.address << " port " << server_port.get_future().get() << endl << endl;

  server_thread.join();

  return 0;

}
