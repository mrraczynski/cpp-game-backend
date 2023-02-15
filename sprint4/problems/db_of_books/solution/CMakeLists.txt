cmake_minimum_required(VERSION 3.11)

project(postgres CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(book_manager src/main.cpp
						   src/json_parser.h
						   src/json_parser.cpp
						   src/db_command.h
						   src/db_command.cpp
						   src/book.h)
target_link_libraries(book_manager PUBLIC CONAN_PKG::libpqxx) 
target_include_directories(book_manager PUBLIC CONAN_PKG::boost)
target_link_libraries(book_manager PUBLIC CONAN_PKG::boost)