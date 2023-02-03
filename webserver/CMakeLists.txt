cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_compile_options(/bigobj)

add_library(model_lib STATIC 
	src/model.h
    src/model.cpp
    src/loot_generator.h
    src/loot_generator.cpp
    src/tagged.h
	src/json_loader.h
	src/json_loader.cpp
	src/collision_detector.h
	src/collision_detector.cpp
	src/geom.h
	src/geom.cpp
	src/model_serialization.h
	src/model_serialization.cpp
	src/postgres.h
	src/postgres.cpp
	src/connection_pool.h
	src/connection_pool.cpp)
target_include_directories(model_lib PUBLIC CONAN_PKG::boost)
target_link_libraries(model_lib PUBLIC CONAN_PKG::boost)
target_link_libraries(model_lib PUBLIC CONAN_PKG::libpq CONAN_PKG::libpqxx)
	
add_library(server_lib STATIC 
	src/sdk.h	
	src/http_server.cpp
	src/http_server.h
	src/logger.h
	src/logger.cpp)	
target_link_libraries(server_lib PUBLIC Threads::Threads)
target_include_directories(server_lib PUBLIC CONAN_PKG::boost)
target_link_libraries(server_lib PUBLIC CONAN_PKG::boost)

add_executable(game_server
	src/main.cpp
	src/sdk.h	
	src/boost_json.cpp	
	src/request_handler.cpp
	src/request_handler.h
	src/api_handler.h
	src/api_handler.cpp
	src/ticker.h
	src/ticker.cpp	
)
target_link_libraries(game_server PRIVATE model_lib)
target_link_libraries(game_server PRIVATE server_lib)

add_executable(game_server_tests    
    tests/model_tests.cpp
    tests/loot_generator_tests.cpp
)
target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2)
target_link_libraries(game_server_tests PRIVATE ${CATCH2_LIB} model_lib)