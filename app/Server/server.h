#pragma once 

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/core/flat_buffer.hpp>

#include "../Router/Router.h"

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;

class Server
{
private:
    boost::system::error_code ec;
    boost::asio::io_context ioc;
    tcp::endpoint ep;
    tcp::acceptor acceptor;
    
    std::atomic<bool> finish;
    std::thread t;

private:
    void connection_handler(tcp::socket&& socket);

    //async work methods
    void do_accept();
    http::request<http::string_body> make_request(std::string &raw_request);

public:
    Router router;
    /**
    * @brief Конструктор сервера
    * 
    * Инициализирует основные поля объекта Server: Boost.Asio endpoint и Boost.Asio acceptor.
    * 
    * @param addr Адрес, по которому сервер будет принимать запросы (например, "127.0.0.1")
    * @param port Номер порта, на котором сервер будет прослушивать входящие соединения
    */
    Server(int port);
    ~Server();
    /**
    * @brief Запускает сервер
    * 
    * Активирует слушание указанного порта и ожидает входящие соединения.
    */
    void start();
    /**
    * @brief Останавливает сервер
    * 
    * Меняет атомарное значение `finish` на `true`, что сигнализирует серверу о необходимости завершения работы.
    * Используется для многопоточного управления командой остановки.
    */
    void stop();

    void close_socket(shared_ptr<tcp::socket> socket);
    /**
    * @brief Запускает сервер
    * 
    * Активирует слушание указанного порта и ожидает входящие соединения в асинхронном режиме.
    */
    void start_async();


public:

};

