/**
 * @file BaseCtrl.h
 * @brief Базовый контроллер для всех контроллеров в приложении.
 *
 * Класс `BaseCtrl` служит основой для всех контроллеров в приложении. Он определяет
 * общую структуру и базовые методы, которые используются всеми контроллерами.
 * Наследование от этого класса позволяет легко расширять функциональность и
 * поддерживать единый стиль разработки.
 */

#pragma once

#include <vector>
#include <jsoncpp/json/json.h>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>

#include "../DataBase/Models/IModel.h"

using namespace boost::beast;

class BaseCtrl
{
protected:
    ModelFactory factory;
    http::request<http::string_body>* request;
    http::response<http::string_body>* response;
public:
    virtual ~BaseCtrl() = default;

    /**
     * @brief Функция создания указателей на ключевые для всех контроллеров обьекты Boost.Beast request и Boost.Beast response
     * 
     * По скольку контроллеры не возвращают готовый ответ, создаются указатели на уже полученный запрос
     * и частично сгенерированный ответ на сервере и изменяют их в процессе своей работы
     * 
     * @param request Boost.Beast request для чтения тела запроса
     * @param response Boost.Beast response для изменения тела, статуса и т.д. ответа
     */
    void set(http::request<http::string_body>* request,
        http::response<http::string_body>* response)
    {
        this->request = request;
        this->response = response;
    }
    
    virtual void index() = 0;
    virtual std::vector<Json::Value> get() = 0;
    virtual void drop() = 0;
};

class UsersCtrl : public BaseCtrl
{
public:
    void index() override;
    std::vector<Json::Value> get() override;
    void drop() override;
    void signup();
    void signin();
};

