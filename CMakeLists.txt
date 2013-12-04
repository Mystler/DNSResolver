cmake_minimum_required(VERSION 2.8)
project(DNSResolver)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_CLANG TRUE)
endif()
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(WARNING_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS "-std=c++0x ${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${WARNING_FLAGS} ${CMAKE_C_FLAGS}")
endif()

set(DNS_HEADERS
    DNS.h
)
set(DNS_SOURCES
    DNS.cpp
    main.cpp
)

if(WIN32)
    add_definitions(-DBUILD_WIN32)
endif()

add_executable(DNSResolver
    ${DNS_HEADERS} ${DNS_SOURCES}
)

if(WIN32)
    target_link_libraries(DNSResolver ws2_32)
endif()
