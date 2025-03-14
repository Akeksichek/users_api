#pragma once

#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace sql;

class DataBase
{
private:
    Driver* driver;
    Connection* connection;

    static DataBase* instance;
    
    DataBase(const char* addr, const char* user, const char* passwd);
public:
    /**
    * @brief Возвращает статическое подключение к базе данных.
    *
    * Гарантирует создание только одного экземпляра подключения к базе данных mysql. Все последующие вызовы
    * этой функции вернут тот же самый экземпляр подключения, что обеспечивает отсутствие дублирующихся
    * подключений и оптимизацию производительности.
    *
    * @param addr Адрес базы данных (например, "localhost").
    * @param user Имя пользователя для аутентификации в базе данных.
    * @param passwd Пароль пользователя для аутентификации в базе данных.
    *
    * @return Указатель на объект DataBase, представляющий подключение к базе данных.
    */
    static DataBase* getInstance(const char* addr, const char* user, const char* passwd);
    /**
     * @brief Функция уничтожения текущего статического подключения
     */
    static void releaseInstance();

    Driver* getDriver();
    Connection* getConnection();

};