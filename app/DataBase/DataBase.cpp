#include "DataBase.h"

DataBase* DataBase::instance = nullptr;

DataBase::DataBase(const char* addr, const char* user, const char* passwd)
    : driver(get_driver_instance()),
      connection(driver->connect(addr, user, passwd)) 
{}

DataBase* DataBase::getInstance(const char* addr, const char* user, const char* passwd)
{
    if(!instance) {
        instance = new DataBase(addr, user, passwd);
    }
    return instance;
}

void DataBase::releaseInstance()
{
    if(instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

Driver* DataBase::getDriver()
{
    return driver;
}
Connection* DataBase::getConnection()
{
    return connection;
}
