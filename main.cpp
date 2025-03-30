#include <iostream>
#include <thread>
#include <chrono>
#include "app/Server/server.h"

#include <boost/beast/http/status.hpp>

Server server(8080);

void start_server()
{
    server.start();
}
void start_async_server()
{
    server.start_async();
}

void stop_server()
{
    std::string input;
    while (input != "stop") {
        std::cout << "Enter 'stop' to terminate the server: ";
        std::cin >> input;
    }
    
    server.stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
    server.router.registerRoute("/users", http::verb::get, new UsersCtrl(), &UsersCtrl::index);
    server.router.registerRoute("/signup", http::verb::post, new UsersCtrl(), &UsersCtrl::signup);
    server.router.registerRoute("/signin", http::verb::post, new UsersCtrl(), &UsersCtrl::signin);

    // std::thread start_thr(start_server);
    std::thread async_start_thr(start_async_server);
    std::thread stop_thr(stop_server);

    // start_thr.join();
    async_start_thr.join();
    stop_thr.join();

    return 0;
}
