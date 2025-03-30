#include "server.h"

Server::Server(int port)
    : ep(boost::asio::ip::tcp::v4(), port),
      ioc(),
      acceptor(ioc, ep),
      finish(false)
{}

void Server::connection_handler(tcp::socket&& socket)
{
    try {
        beast::http::request<beast::http::string_body> request;
        beast::http::response<beast::http::string_body> response;
        
        boost::beast::flat_buffer buffer;
        boost::beast::http::read(socket, buffer, request);

        router.process_handler(&request, &response);

        std::cout << request.target() << std::endl;
        std::cout << request.method_string() << std::endl;
        std::cout << response.result() << std::endl;

        response.version(11);
        response.set(beast::http::field::content_type, "application/json");
        response.prepare_payload();

        beast::http::write(socket, response);
    } catch(const boost::system::system_error& e) {
        if(e.code() == boost::asio::error::eof) {
            std::cerr << "Client disconnected abruptly." << std::endl;
        } else {
            std::cerr << "An error occurred: " << e.what() << std::endl;
        }
    } catch(...) {
        std::cerr << "Unknown exception caught." << std::endl;
    }
}

void Server::start() {
    acceptor.listen();
    while(!finish.load())
    {
        tcp::socket socket(ioc);
        acceptor.accept(socket, ec);
        if(!ec) {
            connection_handler(std::move(socket));
        } else {
            std::cout << ec.message() << std::endl;
        }
    }
}

void Server::stop()
{
    finish.store(true);
    acceptor.close();
}

void Server::start_async() {
    do_accept();

    t = std::thread([&](){ this->ioc.run(); });
    t.join();
}
void Server::do_accept()
{
    auto socket = std::make_shared<tcp::socket>(ioc);

    acceptor.async_accept(*socket.get(), [this, socket = socket](const boost::system::error_code& ec){
        std::array<char, 4096> header_buffer;
        auto buffer = boost::asio::buffer(header_buffer.data(), header_buffer.size());

        socket.get()->async_read_some(buffer, [this, socket = socket, &buffer](const boost::system::error_code& ec, size_t bytes_transfered){
            std::cout.write(reinterpret_cast<const char*>(buffer.data()), bytes_transfered) << std::endl;
            std::string req_str = reinterpret_cast<const char*>(buffer.data());
            auto request = make_request(req_str);
            http::response<http::string_body> response;

            Json::Value json_req;
            Json::Reader reader;
            reader.parse(request.body(), json_req);

            router.process_handler(&request, &response);
            http::write(*socket.get(), response);
        });
        do_accept();
    });
}
http::request<http::string_body> Server::make_request(std::string &raw_request)
{
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

void Server::close_socket(std::shared_ptr<tcp::socket> socket)
{
    if (socket) {
        socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket->close();
    }
}

Server::~Server()
{
    // if (t.joinable()) {
    //     t.join();
    // }
}