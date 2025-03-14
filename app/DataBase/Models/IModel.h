/**
 * @file IModel.h
 * @brief Интерфейс для работы с моделями данных.
 * 
 * Этот интерфейс предоставляет основу для создания различных моделей данных. Каждая модель 
 * наследует этот интерфейс и реализует специфичные для своей таблицы методы. Фабрика моделей 
 * упрощает создание новых экземпляров моделей, устраняя необходимость в ручном создании новой фабрики 
 * для каждой модели. В конструкторе каждой модели передаются аргументы, которые используются для 
 * записи данных в базу данных.
 */

#pragma once

#include "../DataBase.h"
#include "../ModelFactory.h"

#include <jsoncpp/json/json.h>
#include <vector>

class IModel
{
protected:
    ModelFactory factory;

public:
    IModel() = default;
    virtual ~IModel() = default;

    virtual void insert() = 0;
    virtual sql::ResultSet* get() = 0;
    virtual std::vector<Json::Value> get_vector() = 0;
    virtual bool validate_data() = 0;

};

class User : public IModel
{
private:
    int id;
    std::string login, password;
public:
    User() = default;
    User(const std::string& login, const std::string& password) 
        : login(login),
        password(password)
    {}

    bool validate_data() override;
    void insert() override;
    sql::ResultSet* get() override;
    std::vector<Json::Value> get_vector() override;
    Json::Value getUser();
};