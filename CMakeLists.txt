cmake_minimum_required(VERSION 4.3)
project(libarchipelagopp)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS system asio beast json)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)

message(STATUS "${OPENSSL_LIBRARIES}")

add_library(libarchipelagopp STATIC src/ap/archipelago.cpp
        include/ap/archipelago.hpp
        include/ap/client.hpp
        include/ap/protocol.hpp
        include/ap/connection.hpp
        include/ap/clientinfo.hpp
        src/ap/connection.cpp
        src/ap/client.cpp
        src/ap/data_package_cache.cpp
        src/ap/data_package_cache.hpp)
target_include_directories(libarchipelagopp PUBLIC include/)
target_include_directories(libarchipelagopp PRIVATE src/)
target_link_libraries(libarchipelagopp PUBLIC ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
IF(WIN32)
    target_link_libraries(libarchipelagopp PRIVATE OpenSSL::applink)
    target_compile_definitions(libarchipelagopp PUBLIC -D_WIN32_WINNT=0x0A01)
ENDIF()
target_compile_definitions(libarchipelagopp PRIVATE -DBOOST_ASIO_NO_DEPRECATED)
add_library(libarchipelagopp::libarchipelagopp ALIAS libarchipelagopp)

add_subdirectory(example)