project(uitest_server)
cmake_minimum_required(VERSION 3.4.1)

find_package(Boost 1.40.0 REQUIRED COMPONENTS system log)

set(sources
	main.cpp
	StatServer.cpp
	StatConnectionHandler.cpp
	CpuUsage.cpp
	MemUsage.cpp
)

add_executable(uitest_server ${sources})
target_link_libraries(uitest_server Boost::log)
