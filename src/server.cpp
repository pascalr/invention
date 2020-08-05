//#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include <future>

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

#include "lib/serial.h"
#include "lib/linux.h"
#include "core/sweep.h"
//#include "core/fake_serial.h"

#include <unistd.h> // To parse arguments

#include "helper/logging.h"
#include "core/jar_position_analyzer.h"

#include "lib/opencv.h"

#include "lib/NLTemplate.h"

#include "core/WebProgram.h"

#include "controllers/controllers.h"
#include "controllers/recettes_controller.h"
#include "controllers/ingredients_controller.h"
#include "controllers/axes_controller.h"
#include "controllers/layout_controller.h"
#include "controllers/3d_controller.h"

#include "core/Heda.h"
#include "core/writer/serial_writer.h"
//#include "core/writer/command_writer.h"
#include "core/reader/serial_reader.h"
#include "core/reader/shared_reader.h"

using namespace NL::Template;

using namespace std;
using namespace NL::Template;
using namespace boost::property_tree; // json

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

void addPostRoute(HttpServer& server, WebProgram& wp, const char* path, const std::function<void(WebProgram&, PostRequest&)>& func) {
  server.resource[path]["POST"] = [func, &wp](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

    try {
      PostRequest postRequest(request);
      wp.request = request;
      func(wp, postRequest);
    
      if (!postRequest.redirect.empty()) {
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Location", postRequest.redirect);
        response->write(SimpleWeb::StatusCode::redirection_see_other, header);
        return;
      }

    } catch (MissingFieldException& e) {
      cout << "TODO handle missing field exception.";
      return;

    } catch (EmptyMandatoryFieldException& e) {
      cout << "TODO handle empty mandatory field exception.";
      return;

    } catch (std::exception& e) {
      std::cout << "exception: " << e.what() << std::endl;
    }
    
    response->write(SimpleWeb::StatusCode::client_error_forbidden);
  };
}

void addRoute(HttpServer& server, WebProgram& wp, const char* path, const char* method, const std::function<void(WebProgram&, NL::Template::Template&)>& func, string layoutName) {
  server.resource[path][method] = [func, &wp, layoutName](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

    LoaderFile loader;
    Template t( loader );
    
    try {
      wp.request = request;
      func(wp, t);
    } catch (std::exception& e) {
      std::cout << "exception: " << e.what() << std::endl;
    }

    stringstream ss; 
    t.render(ss);

    stringstream withLayout; 
    addLayoutToContent(withLayout, layoutName, ss);

    response->write(withLayout.str()); // OPTIMIZE: Write the stream directly
  };
}

void sendJson(shared_ptr<HttpServer::Response> response, ptree& pt) {

    stringstream ss;
    json_parser::write_json(ss, pt);
                                                                             
    string str = ss.str();
    SimpleWeb::CaseInsensitiveMultimap header;
    header.emplace("Content-Length", to_string(str.length()));
    header.emplace("Content-Type", "application/json");
    response->write(SimpleWeb::StatusCode::success_ok, str, header);
}

//app.use(bodyParser.urlencoded({ extended: true }))
  //res.header("Access-Control-Allow-Origin", "http://localhost:3000");
  //res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  //res.header('Access-Control-Allow-Methods', 'PUT, POST, GET, DELETE, OPTIONS');
//function nocache(req, res, next) {
//  res.header('Cache-Control', 'private, no-cache, no-store, must-revalidate');
//  res.header('Expires', '-1');
//  res.header('Pragma', 'no-cache');
//  next();
//}

int main(int argc, char** argv) {

  boost::log::core::get()->set_filter(
    boost::log::trivial::severity >= boost::log::trivial::trace // SHOW ALL in log file
  );

  boost::log::add_common_attributes();


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
  if (serialPort.openPort("/dev/ttyACM0") < 0) {
    throw InitSerialPortException();
  }
  
  SerialReader serialReader(serialPort);
  SerialWriter serialWriter(serialPort);
  
  SharedReader sharedReader(serialReader);
  SharedReaderClient hedaReader(sharedReader, READER_CLIENT_ID_HEDA);
  SharedReaderClient serverReader(sharedReader, READER_CLIENT_ID_SERVER);

  //CommandWriter writer(sharedReader, serialWriter); 
  //SharedReaderClient reader(sharedReader, READER_CLIENT_ID_HEDA);

  Heda heda(serialWriter, hedaReader); 
  vector<Jar> jars;
  FakeProgram fake;
  WebProgram wp;

  // WARNING: The routes are not in order...
  // So my urls will be longer, but I think I will do it this way:
  // /:model/:action/:id?
 
  auto axesIndex = [&fake](WebProgram& wp, Template& t) {Axes::index(wp, t, fake);};
  addRoute(server, wp, "^/?$", "GET", axesIndex, "frontend/axes/layout.html");
  addRoute(server, wp, "^/axes/index/?$", "GET", axesIndex, "frontend/axes/layout.html");

  addRoute(server, wp, "^/recettes/index/?$", "GET", Recettes::index, "frontend/recettes/layout.html");
  addRoute(server, wp, "^/recettes/new/?$", "GET", Recettes::create, "frontend/recettes/layout.html");
  // FIXME: Match percent too..
  addRoute(server, wp, "^/recettes/show/([\\%_a-zA-Z0-9]+)/?$", "GET", Recettes::show, "frontend/recettes/layout.html");
  addRoute(server, wp, "^/recettes/edit/([\\%_a-zA-Z0-9]+)/?$", "GET", Recettes::edit, "frontend/recettes/layout.html");
  addPostRoute(server, wp, "^/recettes/update/([\\%_a-zA-Z0-9]+)/?$", Recettes::do_update); // FIXME: This should be PUT, be workaround needed since from only supports GET and POST
  addPostRoute(server, wp, "^/recettes/create/?$", Recettes::do_create);
  addRoute(server, wp, "^/recettes/delete/([\\%_a-zA-Z0-9]+)/?$", "GET", Recettes::remove, "frontend/recettes/layout.html"); // FIXME: DELETE, not GET
  
  addRoute(server, wp, "^/ingredients/index/?$", "GET", Ingredients::index, "frontend/ingredients/layout.html"); 
  addRoute(server, wp, "^/ingredients/new/?$", "GET", Ingredients::create, "frontend/ingredients/layout.html");
  addPostRoute(server, wp, "^/ingredients/create/?$", Ingredients::do_create);

  addRoute(server, wp, "^/3d/index.html$", "GET", ThreeD::index, "frontend/3d/layout.html");
 
  // Deprecated: Don't add .html to the end of the url 
  addRoute(server, wp, "^/axes/index.html$", "GET", axesIndex, "frontend/axes/layout.html"); // deprecated
  addRoute(server, wp, "^/ingredients/index.html$", "GET", Ingredients::index, "frontend/ingredients/layout.html"); // deprecated
  addRoute(server, wp, "^/recettes/index.html$", "GET", Recettes::index, "frontend/recettes/layout.html"); // deprecated
  addRoute(server, wp, "^/recettes/new.html$", "GET", Recettes::create, "frontend/recettes/layout.html"); // deprecated
  //addRoute(server, wp, "^/ingredients/show.html$", "GET", Ingredients::show, "frontend/ingredients/layout.html"); // deprecated
  addRoute(server, wp, "^/ingredients/new.html$", "GET", Ingredients::create, "frontend/ingredients/layout.html"); // deprecated

  server.resource["^/run$"]["GET"] = [&heda](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "GET /run" << endl;

    auto query_fields = request->parse_query_string();
    for(auto &field : query_fields) {
      if (field.first == "cmd") {

        heda.execute(field.second);
      }
    }
    response->write("Ok command given to arduino");
  };

  /*server.resource["^/cam_capture.jpg$"]["GET"] = [&p, &jars](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
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

  server.resource["^/listeIngredients$"]["GET"] = [&jars](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "GET /listeIngredients" << endl;
   
    ptree pt;
    pt.put("ingredients", "empty");
    sendJson(response, pt);
  };

  server.resource["^/listeRecettes$"]["GET"] = [&jars](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "GET /listeRecettes" << endl;
   
    ptree pt;
    pt.put("recettes", "empty");
    sendJson(response, pt);
  };

  /*server.resource["^/connect$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "GET /connect" << endl;
    // FIXME: Also handle fake arduino too
    if (p.openPort("/dev/ttyACM0") < 0) {
      response->write("Error opening arduino port.");
      return;
    }
    response->write("Successfully connected to arduino.");
  };

  server.resource["^/close$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    cout << "GET /close" << endl;
    // FIXME: Also handle fake arduino too
    p.closePort();
    response->write("Port closed.");
  };*/
 

  server.resource["^/pollHeda$"]["GET"] = [&heda,&serverReader](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

      string cmd = heda.getCurrentCommand();
      cout << "PollHeda: CurrentCommand: " << cmd << endl;

      ptree pt;
      stringstream ss;
      pt.put("cmd", cmd);
      json_parser::write_json(ss, pt);
                                                                       
      string str = ss.str();
      SimpleWeb::CaseInsensitiveMultimap header;
      header.emplace("Content-Length", to_string(str.length()));
      header.emplace("Content-Type", "application/json");
      response->write(SimpleWeb::StatusCode::success_ok, str, header);
  };

  server.resource["^/poll$"]["GET"] = [&heda,&serverReader](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    thread work_thread([&heda,response,&serverReader] {
      string s;
      while (true) {

        while (serverReader.inputAvailable()) {
          s += (char) serverReader.getByte();
        }
        if (!s.empty()) {
                                                                           
          cout << "Arduino: " << s;
          ptree pt;
          stringstream ss;
          pt.put("log", s);
          json_parser::write_json(ss, pt);
                                                                           
          string str = ss.str();
          SimpleWeb::CaseInsensitiveMultimap header;
          header.emplace("Content-Length", to_string(str.length()));
          header.emplace("Content-Type", "application/json");
          response->write(SimpleWeb::StatusCode::success_ok, str, header);
          return;
        }
        s = "";
        this_thread::sleep_for(chrono::milliseconds(10));
      }
    });
    work_thread.detach();
  };

  // Default GET-example. If no other matches, this anonymous function will be called.
  // Will respond with content in the web/-directory, and its subdirectories.
  // Default file: index.html
  // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
  server.default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical("frontend");
      auto path = boost::filesystem::canonical(web_root_path / request->path);
      // Check if path is within web_root_path
      if(distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
         !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
        throw invalid_argument("path must be within root path");
      if(boost::filesystem::is_directory(path))
        path /= "index.html";

      SimpleWeb::CaseInsensitiveMultimap header;

      // Uncomment the following line to enable Cache-Control
      // header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
//            return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

      auto ifs = make_shared<ifstream>();
      ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

      if(*ifs) {
        auto length = ifs->tellg();
        ifs->seekg(0, ios::beg);

        header.emplace("Content-Length", to_string(length));
        response->write(header);

        // Trick to define a recursive function within this scope (for example purposes)
        class FileServer {
        public:
          static void read_and_send(const shared_ptr<HttpServer::Response> &response, const shared_ptr<ifstream> &ifs) {
            // Read and send 128 KB at a time
            static vector<char> buffer(131072); // Safe when server is running on one thread
            streamsize read_length;
            if((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
              response->write(&buffer[0], read_length);
              if(read_length == static_cast<streamsize>(buffer.size())) {
                response->send([response, ifs](const SimpleWeb::error_code &ec) {
                  if(!ec)
                    read_and_send(response, ifs);
                  else
                    cerr << "Connection interrupted" << endl;
                });
              }
            }
          }
        };
        FileServer::read_and_send(response, ifs);
      }
      else
        throw invalid_argument("could not read file");
    }
    catch(const exception &e) {
      response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
    }
  };

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
  };

  /*thread capture_thread([] {
    Mat frame;
    VideoCapture cap;
  
    if (!initVideo(cap)) {
      cerr << "ERROR! Unable to open camera\n";
      //return -1;
    }
  
    while (true) {
      cap.read(frame);
      if (frame.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        break;
      }
      imwrite("frontend/capture.jpg", frame);
      this_thread::sleep_for(chrono::milliseconds(2000));
    }
    //return 0;
  });
  capture_thread.detach();*/

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
