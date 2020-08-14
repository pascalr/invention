//#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include <future>

#include <vector>
#include "lib/opencv.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
  
bool exiting = false;
pid_t socatPid;

void raspServerSignalHandler( int signum ) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  exiting = true;

  exit(signum);
}

int main(int argc, char** argv) {
  
  signal(SIGINT, raspServerSignalHandler);

  // TODO: Also handle socat the usb

  HttpServer server;
  server.config.address = "192.168.0.19";
  server.config.port = 8889;

  server.resource["^/capture.jpg$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    //cout << "GET /cam_capture" << endl;
    Mat frame;
    captureVideoImage(frame);
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

  pid_t socatPid = fork();

  if(socatPid == 0) { // child process

    while(!exiting) {
      setpgid(getpid(), getpid());
      system("socat /dev/ttyACM0,raw,echo=0,b115200 tcp-listen:8888,reuseaddr");
    }

  } else {   // parent process
    server_thread.join();
  }

  return 0;

}
