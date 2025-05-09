#include "modules/choc/gui/choc_WebView.h"
#include "modules/choc/gui/choc_DesktopWindow.h"
#include "modules/choc/gui/choc_MessageLoop.h"
#include <iostream>
#include "modules/choc/network/choc_HTTPServer.h"

#ifndef STATIC_FILE_DIR 
    #define STATIC_FILE_DIR "/home/leto/ak/xenax/static/"
#endif
// const std::string HTML_FILE = "/home/leto/ak/choc_test_websocket/test.html";
const std::string static_dir = std::string(STATIC_FILE_DIR);
const std::string HTML_FILE = static_dir + "index.html";
// const std::string CSS_FILE = "/home/leto/ak/choc_test_websocket/test.css";
const std::string CSS_FILE = static_dir + "assets/index.css";
const std::string JS_FILE = static_dir + "assets/index.js";

std::atomic<bool> running{true};


choc::network::HTTPServer server;

struct ExampleClientInstance  : public choc::network::HTTPServer::ClientInstance
{
    ExampleClientInstance()
    {
        static int clientCount = 0;
        clientID = ++clientCount;

        std::cout << "New client connected, ID: " << clientID << std::endl;
    }

    ~ExampleClientInstance()
    {
        std::cout << "Client ID " << clientID << " disconnected" << std::endl;
    }

    choc::network::HTTPContent getHTTPContent (std::string_view path) override
    {
        // This path is asking for the default page content
        if (path.starts_with("/?") || path == "/"){
            return choc::network::HTTPContent::forFile(HTML_FILE);
        } else if (path == "/assets/index.css") {
            auto resp = choc::network::HTTPContent::forFile(CSS_FILE);
            // resp.mimeType = "text/css";
            return resp;
        }else if (path == "/assets/index.js") {
            auto resp = choc::network::HTTPContent::forFile(JS_FILE);
            // resp.mimeType = "application/javascript";
            return resp;
        }

        // If you want to serve content for other paths, you would do that here...

        return {};
    }

    void upgradedToWebSocket (std::string_view path) override
    {
        std::cout << "Client ID " << clientID << " opened websocket for path: " << path << std::endl;
    }

    void handleWebSocketMessage (std::string_view message) override
    {
        std::cout << "Client ID " << clientID << " received websocket message: " << message << std::endl;

        // for this demo, we'll just bounce back the same message we received, but
        // obviously this can be anything..
        sendWebSocketMessage (std::string (message) + " (echoed back)");
    }

    int clientID = 0;
};

void test_websocket(const std::vector<std::shared_ptr<ExampleClientInstance>> &clients) {
    while (running.load())
    {
        std::this_thread::sleep_for (std::chrono::seconds (1));
        for (auto& client : clients)
        {
            client->sendWebSocketMessage ("Hello from the server!");
        }
    }
}

int main(){
    std::vector<std::shared_ptr<ExampleClientInstance>> clients;

    choc::ui::setWindowsDPIAwareness(); // For Windows, we need to tell the OS we're high-DPI-aware


    choc::ui::DesktopWindow window_right ({ 100, 100, 600, 600 });

    window_right.setWindowTitle ("Xenax Right");
    window_right.setResizable (true);
    window_right.setMinimumSize (300, 300);
    // window.setMaximumSize (600, 600);
    window_right.windowClosed = [] {
        running.store(false);
        choc::messageloop::stop();
    };

    // create webviews
    choc::ui::WebView webview_right;


    CHOC_ASSERT (webview_right.loadedOK());

    std::string address = "0.0.0.0";
    uint16_t preferredPortNum = 8080;

    window_right.setContent (webview_right.getViewHandle());

    server.open(
        address,
        preferredPortNum,
        1,
        [&clients] () -> std::shared_ptr<ExampleClientInstance>
        {
            auto new_client = std::make_shared<ExampleClientInstance>();
            clients.push_back(new_client);
            return new_client;
        },
        [] (const std::string& error)
        {
            // Handle some kind of server error..
            std::cout << "Error from webserver: " << error << std::endl;
        }

    );

    if(!server.isOpen()){
        std::cout << "ERROR: Server is not open" << std::endl;
    }
    std::cout << "serving on " << server.getHTTPAddress() << std::endl;


    // overwrite target addr with environment var if necessary

    std::string webview_addr = "http://127.0.0.1:" +  std::to_string(server.getPort()) + "/?screen=1";


    char* webview_addr_env = std::getenv("XENAX_WEBVIEW_TARGET");
    if (webview_addr_env != nullptr) webview_addr = webview_addr_env;

    std::cout << "pointing webview to " << webview_addr << std::endl;
    
    webview_right.navigate( webview_addr );


    window_right.toFront();

    

    // std::cout << "Starting hellos from server" << std::endl;

    // std::thread serverThread([&clients] {
    //     test_websocket(clients);
    // });

    std::cout << "Starting message loop" << std::endl;
    choc::messageloop::run();

    // Join the thread before exiting main
    // serverThread.join();

    return 0;
}