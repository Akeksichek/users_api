#include <unordered_map>
#include <functional>
#include <memory>

#include "../Controllers/BaseCtrl.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>

using namespace boost::beast;
using namespace std;

struct RouteHandler {
    using ctrlMethodPtr = std::function<void()>;

    http::verb method;
    BaseCtrl* controller;
    ctrlMethodPtr cmptr;
};

class Router {
private:
    std::unordered_multimap<std::string, RouteHandler> routes;

public:
    template <typename TCtrl, typename TCtrlMethod>
    /**
    * @brief Регистрация нового маршрута и привязка метода контроллера к нему.
    *
    * Эта функция создает новый маршрут для обработки HTTP-запросов и связывает его с методом конкретного контроллера.
    * Контроллер — это объект, содержащий бизнес-логику приложения, а метод контроллера — это конкретная функция,
    * которая будет выполнена при обращении к зарегистрированному маршруту.
    *
    * @param route Путь, по которому будут поступать запросы пример: "localhost:port/users".
    * @param method Метод HTTP, который этот маршрут поддерживает-GET, POST
    * @param ctrl Указатель на экземпляр контроллера, содержащего метод-обработчик.
    * @param CtrlMethodPtr Указатель на метод контроллера, который будет вызван при обработке запроса.
    */
    void registerRoute(const std::string& route, http::verb method, TCtrl* ctrl, void (TCtrlMethod::*CtrlMethodPtr)())
    {
        routes.emplace(route, RouteHandler{method,
            ctrl,
            std::bind(CtrlMethodPtr, ctrl)});
        std::cout << "\nRoute was created\n";
    }

    /**
    * @brief Обработка входящего HTTP-запроса.
    *
    * Эта функция ищет зарегистрированный маршрут, соответствующий переданному запросу, и вызывает связанный с ним метод контроллера.
    * Если маршрут не найден или метод запроса не совпадает с ожидаемым, возвращается соответствующая ошибка - HTTP-статус 404 или 405.
    *
    * @param request Указатель на объект запроса Boost.Beast.
    * @param response Указатель на объект ответа Boost.Beast.
    */
    void process_handler(http::request<http::string_body>* request, http::response<http::string_body>* response) {
        if (routes.find(request->target()) == routes.end()) {
            response->result(http::status::not_found);
            return;
        }

        auto range = routes.equal_range(request->target());
        for (auto it = range.first; it != range.second; ++it) {
            const auto& route = it->second;
            if (request->method() == route.method) {
                response->result(http::status::ok);
                route.controller->set(request, response);
                route.cmptr();
                return;
            }
        }
        response->result(http::status::method_not_allowed);
    }
};