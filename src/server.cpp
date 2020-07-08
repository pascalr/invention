#include "client_http.hpp"
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

using namespace std;
// Added for the json-example:
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

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

  setupLogging();

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

  // optind: The index that has been read up to there

  if (serverAddress.empty() || serverAddress == "localhost") {
    serverAddress = "127.0.0.1";
  } else if (serverAddress == "lan") {
    getLanAddress(serverAddress);
  }

  HttpServer server;
  server.config.address = serverAddress;
  server.config.port = serverPort ? serverPort : 8083;
  
  //cout << "Serveraddress: " << serverAddress << endl;
  //cout << "Port: " << server.config.port << endl;
  
  SerialPort p;
  vector<Jar> jars;
  //FakeSerialPort fake;

/*  server.resource["^/run/arduino([0-9]+)$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    response->write(request->path_match[1].str());
  };
  server.resource["^/run/arduino$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
app.get('/run/arduino',function (req, res) {
  log('Running command arduino: ' + req.query.cmd)
  arduino_or_fake.write(req.query.cmd+"\n")
  res.end()
})*/
  server.resource["^/run/arduino$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    if (!p.isOpen()) {
      response->write("Error arduino is not connected.");
      return;
    }
    
    auto query_fields = request->parse_query_string();
    for(auto &field : query_fields) {
      if (field.first == "cmd") {
        if (field.second == "info") { // FIXME: Temporary fix until ?cmd=? works
          p.writePort("?");
        } else {
          p.writePort(field.second);
        }
      }
    }
    response->write("Ok command given to arduino");
  };

  server.resource["^/sweep$"]["GET"] = [&p, &jars](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    if (!p.isOpen()) {
      response->write("Error arduino is not connected.");
      return;
    }
    Sweep sweep(p);
    if(!sweep.init()) {
      response->write("Error initializing sweep.");
      return;
    }
    sweep.run(jars);
    
    response->write("Ok executing command sweeep");
  };

  server.resource["^/connect$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    // FIXME: Also handle fake arduino too
    if (p.openPort("/dev/ttyACM0") < 0) {
      response->write("Error opening arduino port.");
      return;
    }
    response->write("Successfully connected to arduino.");
  };

  server.resource["^/close$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    // FIXME: Also handle fake arduino too
    p.closePort();
  };
 
  server.resource["^/poll$"]["GET"] = [&p](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    if (p.isOpen()) {
      thread work_thread([&p,response] {
        while (!p.inputAvailable()) {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
        string s;
        p.getInput(s);
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
      });
      work_thread.detach();
    }
  };

  //server.resource["^/info$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

    //if (!p.isOpen()) {
    //  response->write("Error arduino is not connected.");
    //  return;
    //}
    //p.writePort(field.second);

    /*if (p.isOpen()) {
      thread work_thread([&p,response] {
        while (!p.inputAvailable()) {
          this_thread::sleep_for(chrono::milliseconds(10));
        }
        string s;
        p.getInput(s);
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
      });
      work_thread.detach();
    }

    stringstream stream;
    stream << "<h1>Request from " << request->remote_endpoint().address().to_string() << ":" << request->remote_endpoint().port() << "</h1>";

    stream << request->method << " " << request->path << " HTTP/" << request->http_version;

    stream << "<h2>Query Fields</h2>";
    auto query_fields = request->parse_query_string();
    for(auto &field : query_fields)
      stream << field.first << ": " << field.second << "<br>";

    stream << "<h2>Header Fields</h2>";
    for(auto &field : request->header)
      stream << field.first << ": " << field.second << "<br>";

    response->write(stream);*/
  //};

  // GET-example for the path /match/[number], responds with the matched string in path (number)
  // For instance a request GET /match/123 will receive: 123
  server.resource["^/match/([0-9]+)$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    response->write(request->path_match[1].str());
  };

  // GET-example simulating heavy work in a separate thread
  server.resource["^/work$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
    thread work_thread([response] {
      this_thread::sleep_for(chrono::seconds(5));
      response->write("Work done");
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
