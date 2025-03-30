#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <thread>
#include <boost/container/pmr/memory_resource.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>
#include <array>

#include <jsoncpp/json/json.h>

using namespace boost::asio;
using ip::tcp;
using namespace boost::beast;

class Server {
public:
    Server(int port)
        : acceptor(ioc, tcp::endpoint(tcp::v4(), port))
    {}

    void run() {
        do_accept();

        std::thread t([this]() { ioc.run(); });
        t.join();
    }

private:
void do_accept() {
    auto socket = std::make_shared<tcp::socket>(ioc);

    acceptor.async_accept(*socket.get(), [this, socket = socket](const boost::system::error_code& ec){
        std::array<char, 4096> header_buffer;
        auto buffer = boost::asio::buffer(header_buffer.data(), header_buffer.size());

        socket.get()->async_read_some(buffer, [this, socket = socket, &buffer](const boost::system::error_code& ec, size_t bytes_transfered){
            std::cout.write(reinterpret_cast<const char*>(buffer.data()), bytes_transfered) << std::endl;
            std::string req_str = reinterpret_cast<const char*>(buffer.data());
            auto request = make_request(req_str);

            Json::Value json_req;
            Json::Reader reader;
            reader.parse(request.body(), json_req);

            std::cout << json_req.toStyledString() << std::endl;
        });
        do_accept();
    });

}

http::request<http::string_body> make_request(std::string &raw_request) {
    http::request<http::string_body> req;
    auto start_pos = raw_request.find(" ");
    if (start_pos != std::string::npos) {
        req.method_string(raw_request.substr(0, start_pos));
        auto end_pos = raw_request.find(" ", start_pos + 1);
        if (end_pos != std::string::npos) {
            req.target(raw_request.substr(start_pos + 1, end_pos - start_pos - 1));
            req.version(11);

            size_t header_start = raw_request.find("\r\n");
            while (header_start != std::string::npos && header_start > 0) {
                size_t header_end = raw_request.find("\r\n", header_start + 2);
                if (header_end == std::string::npos)
                    break;
                std::string header_line = raw_request.substr(header_start + 2, header_end - header_start - 2);
                size_t colon_pos = header_line.find(":");
                if (colon_pos != std::string::npos) {
                    std::string key = header_line.substr(0, colon_pos);
                    std::string value = header_line.substr(colon_pos + 1);
                    req.set(key, value);
                }
                header_start = header_end;
            }
        }
    }
    size_t separator_position = raw_request.find("\r\n\r\n");
    if (separator_position != std::string::npos) {
        std::string body_part = raw_request.substr(separator_position + 4);
        req.body() = body_part;
    }
    return req;
}

private:
    io_context ioc;
    tcp::acceptor acceptor;
};

int main() {
    Server server(3030);
    server.run();

    return 0;
}