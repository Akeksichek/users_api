#include "server.h"

Server::Server(const std::string& addr, int port)
    : ep(boost::asio::ip::address::from_string(addr), port),
      acceptor(ioc, ep),
      finish(false)
{}

void Server::connection_handler(tcp::socket&& socket)
{
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
}

void Server::start() {
    acceptor.listen();
    while(!finish.load())
    {
        boost::asio::ip::tcp::socket socket(ioc);
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

void Server::connection_handler_async(tcp::socket&& socket)
{
    beast::http::request<beast::http::string_body> request;
    beast::http::response<beast::http::string_body> response;
    boost::beast::flat_buffer buffer;

    boost::beast::http::async_read(socket, buffer, request,
        [this, &socket, &buffer, &request](const boost::system::error_code& ec, size_t bytes_transferred) {
            if (ec) {
                std::cerr << "Error reading request: " << ec.message() << std::endl;
                return;
            }});
            router.process_handler(&request, &response);

    std::cout << request.target() << std::endl;
    std::cout << request.method_string() << std::endl;
    std::cout << response.result() << std::endl;

    response.version(11);
    response.set(beast::http::field::content_type, "application/json");
    response.prepare_payload();

    boost::beast::http::async_write(socket, response,
        [this, &socket, &buffer, &response, &request](const boost::system::error_code& ec, size_t bytes_transferred) {
            if (ec) {
                std::cerr << "Error writing response: " << ec.message() << std::endl;
            }

            if (request.need_eof()) {
                socket.shutdown_both;
                if (ec && ec != boost::system::errc::not_connected) {
                    std::cerr << "Error shutting down the socket: " << ec.message() << std::endl;
                }
            }

            buffer.consume(buffer.size());
        });
}

void Server::start_async() {
    acceptor.listen();

    do {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc);

        acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
            if (!ec) {
                connection_handler_async(std::move(*socket));
            } else {
                std::cerr << "Error accepting connection: " << ec.message() << std::endl;
            }
        });
    } while (!finish.load());
}