CXX = g++
CXXFLAGS += -std=c++17

.PHONY: build run-server

build:
	g++ ./route_server/server.cpp -o ./build/route_server.out

run-server: build
	./build/route_server.out
