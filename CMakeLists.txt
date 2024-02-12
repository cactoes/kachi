cmake_minimum_required(VERSION 3.20)
project(kachi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp-html-parser)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp-httplib)
target_link_libraries(${PROJECT_NAME} PRIVATE cpp-html-parser)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /D "_MBCS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oxs")
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++20")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
    target_link_libraries(${PROJECT_NAME} PRIVATE crypt32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std=c++20")
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()