CXX = g++
PATH_HOME = $(shell pwd)
# Warning flags to suppress.
WARNING_FLAGS = -Wno-deprecated-declarations
# Use c++17.
# Include the grpc headers.
HEADERS_GRPC_INCLUDE = -I/opt/grpc/include
HEADERS_CgRPC = -I$(PATH_HOME)/utils_protos/gen -I$(PATH_HOME)/utils_database -I$(PATH_HOME)/src_server/server
CXXFLAGS += -std=c++17 $(HEADERS_GRPC_INCLUDE) $(HEADERS_CgRPC) ${WARNING_FLAGS}
# Source files
SOURCE_FILES = $(wildcard ./utils_database/*.cpp) $(wildcard ./utils_protos/gen/*.cc) $(wildcard ./src_server/server/*.cpp)
# LD = Link the dependencies.
LDFLAGS += -L/opt/grpc/lib

# -Wl = Pass everything after this comma to the linker (not the compiler), without this, g++ would try to interpret --start-group as its own flag and complain.
# --start-group = Tells the linker that the libraries between this and --end-group may reference each other in any order.
# wildcard = A make function that expands to every file in /opt/grpc/lib/ matching the pattern lib*.a, thus no manual input needed for each library.
# -lpthread = Links against libpthread.so (POSIX threads).
# -ldl = Links against libdl.so (dynamic loading).
# -lz = Links against libz.so (zlib compression).
# .a = Stands for archive which is the standard extension for a static library on Unix/Linux.
# .so = Shared (dynamic library) means code stays in a separate file (The binary just references it at runtime).
LDLIBS += -Wl,--start-group $(wildcard /opt/grpc/lib/lib*.a) -Wl,--end-group -lpthread -ldl -lz

# Define the headers necessary for protobuf usage in C++.
PROTOC = protoc
PROTOS_PATH = ./utils_protos

.PHONY: build-protoc build-server build run-server

build-proto:
	${PROTOC} -I $(PROTOS_PATH) --cpp_out=${PROTOS_PATH}/gen --grpc_out=${PROTOS_PATH}/gen --plugin=protoc-gen-grpc=/opt/grpc/bin/grpc_cpp_plugin utils_protos/route.proto

build-server:
	${CXX} ${CXXFLAGS} ./src_server/main.cpp ${SOURCE_FILES} -o ./build/src_server.out ${LDFLAGS} ${LDLIBS}

build: build-server

run-server: build
	clear
	# Should be a relative path.
	# Sample script: make run-server DB_RELATIVE_PATH=utils_database/database.json
	./build/src_server.out --db_path=$(PATH_HOME)/$(DB_RELATIVE_PATH)
