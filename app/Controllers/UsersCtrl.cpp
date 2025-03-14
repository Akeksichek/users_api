#include "BaseCtrl.h"

void UsersCtrl::index()
{
    User user;
    Json::Value json_resp(Json::arrayValue);
    Json::Reader reader;

    auto users = user.get_vector();
    for(auto user : users) {
        json_resp.append(user);
    }

    response->body() = json_resp.toStyledString();
}

std::vector<Json::Value> UsersCtrl::get()
{
    std::cout << "\nUsersCtrl::get() was worked\n";
}

void UsersCtrl::drop()
{
    std::cout << "\nUsersCtrl::drop() was worked\n";
}

void UsersCtrl::signup()
{
    Json::Value json_req;
    Json::Reader reader;
    reader.parse(request->body(), json_req);

    User user(json_req["login"].asString(), json_req["password"].asString());
    if(!user.validate_data()) {
        response->result(http::status::bad_request);
        return;
    }

    user.insert();
    response->body() = "ok";
}

void UsersCtrl::signin()
{
    Json::Value json_req;
    Json::Reader reader;
    reader.parse(request->body(), json_req);
    User user(json_req["login"].asString(), json_req["password"].asString());

    auto user_data = user.getUser();

    if(user_data["error"]) {
        Json::Value json_resp;
        json_resp["error"] = user_data["error"];
        response->body() = json_resp.toStyledString();
        return;
    }

    if(user_data["password"] != json_req["password"]) {
        Json::Value json_resp;
        json_resp["error"] = "Пароль не верный";
        response->body() = json_resp.toStyledString();
        return;
    }
    response->body() = json_req.toStyledString();
}