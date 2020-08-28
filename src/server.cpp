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
#include "core/heda.h"
#include "utils/io_common.h"

#include "lib/opencv.h"

#include "core/writer/serial_writer.h"
#include "core/writer/log_writer.h"
//#include "core/writer/command_writer.h"
#include "core/reader/serial_reader.h"
#include "core/reader/shared_reader.h"
#include "core/reader/log_reader.h"
#include "core/two_way_stream.h"
#include "core/serialize.h"
#include "core/heda_controller.h"

using namespace std;
using namespace boost::property_tree; // json

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

//void sendJson(shared_ptr<HttpServer::Response> response, ptree& pt) {
void sendJson(shared_ptr<HttpServer::Response> response, stringstream& ss) {

    string str = ss.str();
    SimpleWeb::CaseInsensitiveMultimap header;
    header.emplace("Content-Length", to_string(str.length()));
    header.emplace("Content-Type", "application/json");
    response->write(SimpleWeb::StatusCode::success_ok, str, header);
}


int main(int argc, char** argv) {
  
  signal(SIGINT, signalHandler);

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

  SerialPort serialPort;
  if (serialPort.openPort("/tmp/heda0") < 0) {
    throw InitSerialPortException();
  }
  
  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);
  
  SharedReader sharedReader(serialReader);
  SharedReaderClient hedaReader(sharedReader, READER_CLIENT_ID_HEDA);
  SharedReaderClient serverReader(sharedReader, READER_CLIENT_ID_SERVER);

  TwoWayStream serverStream;
  TwoWayStream hedaToServerStream;
  LogWriter hedaToServerLogStream("\033[37mTo server\033[0m", hedaToServerStream);

  LogWriter hedaLogWriter("\033[33mTo slave\033[0m", serialWriter);
  LogReader hedaLogReader("\033[34mFrom slave\033[0m", hedaReader);

  Database db(DB_PROD);
  Heda heda(hedaLogWriter, hedaLogReader, db, hedaToServerStream); 

  // THIS SHOULD BE THE ONLY RESSOURCE. LATER DELETE ALL OTHERS. EHH, ALSO POLL!!
  server.resource["^/run$"]["POST"] = [&heda,&serverStream](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "POST /run" << endl;

    SimpleWeb::CaseInsensitiveMultimap vals;
    vals = SimpleWeb::QueryString::parse(request->content.string());

    for(auto &field : vals) {
      if (field.first == "cmd") {
        serverStream << field.second.c_str();
        /*try {
          heda.execute(field.second);
        } catch (const exception& e) {
          cout << "Caught an exception: " << e.what() << endl;
          std::exception_ptr p = std::current_exception();
          std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
        } catch (...) {
          cout << "Caught an exception which does not inherit exception class." << endl;
          std::exception_ptr p = std::current_exception();
          std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
        }*/
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

    /*stringstream ss;
    ss << "{";
    writeJson(ss, "pos", heda.getPosition());
    writeJson(ss, "toolPos", heda.getToolPosition());
    writeJson(ss, "pending", heda.getPendingCommands());
    writeJson(ss, "output", getAllAvailable(serverReader));
    ss << "\"" << "gripped_jar_format_id" << "\": " << heda.gripped_jar.jar_format_id;
    ss << "}";*/

    ptree pt;
    pt.put("pos", heda.getPosition());
    pt.put("toolPos", heda.getToolPosition());
    pt.put("pending", heda.getPendingCommands());
    pt.put("output", getAllAvailable(serverReader));
    pt.put("isPaused", heda.is_paused);
    pt.put("gripped_jar_format_id", heda.gripped_jar.jar_format_id);
    stringstream ss;
    json_parser::write_json(ss, pt);

    sendJson(response, ss);
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

  //server_thread.join();
  LogReader serverLogReader("\033[36mFrom server\033[0m", serverStream);

  heda.sync();

  HedaController c(heda);
  c.loopCommandStack(serverLogReader);
}
