cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(nes VERSION 0.1.0)


if(NOT TARGET spdlog)
	find_package(spdlog REQUIRED)
endif()


include_directories(include)
add_executable(nes ${PROJECT_SOURCE_DIR}/src/nes.cpp)
target_link_libraries(nes PRIVATE spdlog::spdlog_header_only)
