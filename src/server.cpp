//#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include <future>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <unistd.h> // To parse arguments
#include <vector>
#include "lib/serial.h"
#include "lib/linux.h"
#include "helper/logging.h"
#include "core/jar_position_analyzer.h"
#include "core/Heda.h"

#include "lib/opencv.h"

#include "core/writer/serial_writer.h"
//#include "core/writer/command_writer.h"
#include "core/reader/serial_reader.h"
#include "core/reader/shared_reader.h"
#include "core/reader/io_reader.h"
#include "core/writer/console_writer.h"

using namespace std;
using namespace boost::property_tree; // json

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

void sendJson(shared_ptr<HttpServer::Response> response, ptree& pt) {

    stringstream ss;
    json_parser::write_json(ss, pt);
                                                                             
    string str = ss.str();
    SimpleWeb::CaseInsensitiveMultimap header;
    header.emplace("Content-Length", to_string(str.length()));
    header.emplace("Content-Type", "application/json");
    response->write(SimpleWeb::StatusCode::success_ok, str, header);
}

int main(int argc, char** argv) {

  setupLogging();

  // ************** PARSE ARGS *******************
  string serverAddress;
  int serverPort = 0;
  int opt;

  while ((opt = getopt(argc, argv, "p:a:")) != -1) {
    switch (opt) {
    case 'p':
      serverPort = atoi(optarg);
      break;
    case 'a':
      serverAddress = optarg;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-p port] [-a address]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (serverAddress.empty() || serverAddress == "localhost") {
    serverAddress = "127.0.0.1";
  } else if (serverAddress == "lan") {
    getLanAddress(serverAddress);
  }
  
  // ************** SERVER CONFIG *******************

  HttpServer server;
  server.config.address = serverAddress;
  server.config.port = serverPort ? serverPort : 8083;

#ifdef SIMULATION
  ConsoleWriter serialWriter;
  IOReader serialReader;
#else
  SerialPort serialPort;
  if (serialPort.openPort("/dev/ttyACM0") < 0) {
    throw InitSerialPortException();
  }
  
  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);
#endif

  
  SharedReader sharedReader(serialReader);
  SharedReaderClient hedaReader(sharedReader, READER_CLIENT_ID_HEDA);
  SharedReaderClient serverReader(sharedReader, READER_CLIENT_ID_SERVER);

  Database db("data/test.db");
  Heda heda(serialWriter, hedaReader, db); 

  // THIS SHOULD BE THE ONLY RESSOURCE. LATER DELETE ALL OTHERS. EHH, ALSO POLL!!
  server.resource["^/run$"]["POST"] = [&heda](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "POST /run" << endl;

    SimpleWeb::CaseInsensitiveMultimap vals;
    vals = SimpleWeb::QueryString::parse(request->content.string());

    for(auto &field : vals) {
      if (field.first == "cmd") {
        heda.execute(field.second);
      }
    }
    response->write("ok");
  };

  /*server.resource["^/cam_capture.jpg$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
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
  };*/

  server.resource["^/poll$"]["GET"] = [&heda,&serverReader](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

    ptree pt;
    pt.put("pos", heda.getPosition());
    pt.put("cmd", heda.getCurrentCommand());
    pt.put("pending", heda.getPendingCommands());
    pt.put("output", getAllAvailable(serverReader));
    sendJson(response, pt);
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
  cout << "Server listening on " << serverAddress << " port " << server_port.get_future().get() << endl << endl;

  server_thread.join();
}
