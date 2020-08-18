//#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include <future>

#include <vector>
#include "lib/opencv.h"

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
  
int main(int argc, char** argv) {

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

  server_thread.join();

  return 0;

}
