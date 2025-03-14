cmake_minimum_required(VERSION 3.5)
project(api_cpp)

include_directories(/usr/include/jsoncpp/json)
include_directories(/usr/include/mysql-cppconn-8/)

find_package(jsoncpp REQUIRED)

set(SOURCE
    main.cpp
    app/Server/server.h app/Server/server.cpp

    app/Router/Router.h

    app/Controllers/BaseCtrl.h 
        app/Controllers/UsersCtrl.cpp

    app/DataBase/DataBase.h app/DataBase/DataBase.cpp
    /usr/include/mysql_connection.h
    app/DataBase/ModelFactory.h
    app/DataBase/Models/IModel.h 
        app/DataBase/Models/User.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} PUBLIC jsoncpp mysqlcppconn)
