CXX = g++
PATH_HOME = ${shell pwd}
WARNING_FLAGS = -Wno-deprecated-declarations
CPP_STD = -std=c++17

# ---------------------------------------------------------------------------------------------- #
# This are headers and source files that can be included both in client and server source files. #
# ---------------------------------------------------------------------------------------------- #
HEADERS_GRPC_INCLUDE = -I/opt/grpc/include
HEADERS_UTILS_PROTOS = -I${PATH_HOME}/utils_protos/gen
HEADERS_UTILS_DATABASE = -I${PATH_HOME}/utils_database
SOURCE_FILES = ${wildcard ./utils_database/*.cpp} ${wildcard ./utils_protos/gen/*.cc}

# ----------------------------------------- #
# Server specific headers and source files. #
# ----------------------------------------- #
SERVER_HEADERS_CgRPC = ${HEADERS_UTILS_PROTOS} ${HEADERS_UTILS_DATABASE} -I${PATH_HOME}/src_server/server
SERVER_SOURCE_FILES = ${SOURCE_FILES} ${wildcard ./src_server/server/*.cpp}
SERVER_CXXFLAGS += ${CPP_STD} ${HEADERS_GRPC_INCLUDE} ${SERVER_HEADERS_CgRPC} ${WARNING_FLAGS}

# ----------------------------------------- #
# CLient specific headers and source files. #
# ----------------------------------------- #
CLIENT_HEADERS_CgRPC = ${HEADERS_UTILS_PROTOS} ${HEADERS_UTILS_DATABASE} -I${PATH_HOME}/src_client/client
CLIENT_SOURCE_FILES = ${SOURCE_FILES} ${wildcard ./src_client/client/*.cpp}
CLIENT_CXXFLAGS += ${CPP_STD} ${HEADERS_GRPC_INCLUDE} ${CLIENT_HEADERS_CgRPC} ${WARNING_FLAGS}

# ------------------------------------------------------------------------------------------------------------------------------------------------------------- #
# -Wl = Pass everything after this comma to the linker {not the compiler}, without this, g++ would try to interpret --start-group as its own flag and complain. #
# --start-group = Tells the linker that the libraries between this and --end-group may reference each other in any order.																				#
# wildcard = A make function that expands to every file in /opt/grpc/lib/ matching the pattern lib*.a, thus no manual input needed for each library.						#
# -lpthread = Links against libpthread.so {POSIX threads}. 																																																			#
# -ldl = Links against libdl.so {dynamic loading}.																																																							#
# -lz = Links against libz.so {zlib compression}.																																																								#
# .a = Stands for archive which is the standard extension for a static library on Unix/Linux.																																		#
# .so = Shared {dynamic library} means code stays in a separate file {The binary just references it at runtime}.																								#
# ------------------------------------------------------------------------------------------------------------------------------------------------------------- #
LDLIBS += -Wl,--start-group ${wildcard /opt/grpc/lib/lib*.a} -Wl,--end-group -lpthread -ldl -lz
# --------------------------- #
# LD = Link the dependencies. #
# --------------------------- #
LDFLAGS += -L/opt/grpc/lib

# ------------------------------------------------------- #
# Define the headers necessary for protobuf usage in C++. #
# ------------------------------------------------------- #
PROTOC = protoc
PROTOS_PATH = ./utils_protos

.PHONY: build-protoc build-server build-client run-server

build-proto:
	${PROTOC} -I ${PROTOS_PATH} --cpp_out=${PROTOS_PATH}/gen --grpc_out=${PROTOS_PATH}/gen --plugin=protoc-gen-grpc=/opt/grpc/bin/grpc_cpp_plugin utils_protos/route.proto

build-server: build-proto
	${CXX} ${SERVER_CXXFLAGS} ./src_server/main.cpp ${SERVER_SOURCE_FILES} -o ./build/src_server.out ${LDFLAGS} ${LDLIBS}

build-client: build-proto
	${CXX} ${CLIENT_CXXFLAGS} ./src_client/main.cpp ${CLIENT_SOURCE_FILES} -o ./build/src_client.out ${LDFLAGS} ${LDLIBS}

run-server: build-server
	clear
	./build/src_server.out --db_path=${PATH_HOME}/utils_database/database.json

run-client: build-client
	clear
	./build/src_client.out --db_path=${PATH_HOME}/utils_database/database.json
