#include "IModel.h"

void User::insert()
{
    if(!validate_data()) {
        return;
    }
    PreparedStatement* pstmt = factory.connection->prepareStatement("INSERT INTO users(login, password) VALUES (?, ?)");
    pstmt->setString(1, login);
    pstmt->setString(2, password);

    int affectedRows = pstmt->executeUpdate();
    
    delete pstmt;
}

sql::ResultSet* User::get()
{
    sql::ResultSet* result;
    return result;
}

std::vector<Json::Value> User::get_vector()
{
    std::vector<Json::Value> users;

    PreparedStatement* pstmt = factory.connection->prepareStatement("SELECT * FROM users");
    sql::ResultSet* result_set = pstmt->executeQuery();

    while(result_set->next()) {
        Json::Value user;
        user["id"] = result_set->getString("id").asStdString();
        user["login"] = result_set->getString("login").asStdString();
        user["password"] = result_set->getString("password").asStdString();
        users.push_back(user);
    }

    return users;
}

Json::Value User::getUser()
{
    Json::Value res;

    PreparedStatement* pstmt = factory.connection->prepareStatement("SELECT * FROM users WHERE login = ?");
    pstmt->setString(1, login);
    sql::ResultSet* result_set = pstmt->executeQuery();

    if(!result_set->next()) {
        res["error"] = "ПОльзователь не найден";
    } else {
        res["login"] = result_set->getString("login").asStdString();
        res["password"] = result_set->getString("password").asStdString();
    }

    return res;
}

bool User::validate_data()
{
    if(login.empty() || password.empty()) {
        return false;
    }
    if(login.size() < 3 || password.size() < 3) {
        return false;
    }
    return true;
}