project(uitest_server)
cmake_minimum_required(VERSION 3.4.1)

find_package(Boost 1.40.0 REQUIRED COMPONENTS system)

set(sources
	main.cpp
)

add_executable(uitest_client ${sources})

