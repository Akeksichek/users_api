#pragma once 

#include <iostream>
#include <atomic>

#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

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

private:
    void connection_handler(tcp::socket&& socket);
    
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
    Server(const std::string& addr, int port);
    ~Server() = default;
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

    void start_async();
    void connection_handler_async(tcp::socket&& socket);


public:

};

