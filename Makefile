CXX = g++
CXXFLAGS += -std=c++17

.PHONY: build run-server

build:
	${CXX} ./route_server/server.cpp -o ./build/route_server.out

run-server: build
	./build/route_server.out
