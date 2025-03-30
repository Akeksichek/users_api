/**
 * @file ModelFactory.h
 * @brief Обьект позволяющий получить поключение к базе данных
 * 
 * Позволяет получать доступ к важным обьектам для отправки запросов в бд, с указанным в конструкторе настройками,
 * также гарантирует автоматическую очистку подключения
 */

#pragma once

#include "DataBase.h"

class ModelFactory
{
private:
    DataBase* m_db;
public:
    Driver* driver;
    Connection* connection;

    ModelFactory()
        : m_db(DataBase::getInstance("tcp://127.0.0.1:3306", "root", "root")),
          driver(m_db->getDriver()),
          connection(m_db->getConnection())
    {
        connection->setSchema("auth_reg_boost_mysql_connector");
    }
    ~ModelFactory() {
        DataBase::releaseInstance();
    }
};
