CXX = g++
# Use c++17.
# Include the grpc headers.
CXXFLAGS += -std=c++17 -I/opt/grpc/include
# Source files
SOURCE_FILES = $(wildcard ./utils/*.cpp)
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

.PHONY: build run-server

build-server:
	${CXX} ${CXXFLAGS} ./src_server/server.cpp ${SOURCE_FILES} -o ./build/src_server.out ${LDFLAGS} ${LDLIBS}

build: build-server

run-server: build
	clear
	./build/src_server.out
